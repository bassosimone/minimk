// File: libminimk/runtime/runtime.cpp
// Purpose: cooperative coroutine runtime implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/poll.h>    // for minimk_poll
#include <minimk/runtime.h> // for minimk_runtime_run
#include <minimk/time.h>    // for minimk_time_monotonic_now

#include <limits.h> // for INT_MAX
#include <stddef.h> // for size_t
#include <stdint.h> // for uintptr_t
#include <string.h> // for memset

#include "assert.h" // for MINIMK_ASSERT
#include "stack.h"  // for struct stack
#include "switch.h" // for minimk_switch
#include "trace.h"  // for MINIMK_TRACE

#include "../integer/u64.h" // for minimk_integer_u64_satadd

/// Unused coroutine slot.
#define CORO_NULL 0

/// Coroutine can run immediately.
#define CORO_RUNNABLE 1

/// Coroutine has terminated.
#define CORO_EXITED 2

/// Coroutine is blocked awaiting for a deadline.
#define CORO_BLOCKED_ON_TIMER 3

/// Portable coroutine state.
///
/// We align this structure to safely memset it to zero on arm64.
struct coroutine {
    /// Saved stack pointer.
    uintptr_t sp;

    /// Allocated coroutine stack.
    struct stack stack;

    /// Entry point.
    void (*entry)(void *opaque);
    void *opaque;

    /// State.
    unsigned long state;

    /// Management of the blocked state.
    uint64_t deadline;

} __attribute__((aligned(16)));

/// Maximum number of coroutines we can create.
#define MAX_COROS 16

/// Slots for coroutines we manage.
static coroutine coroutines[MAX_COROS];

/// Pointer to currently running coroutine.
static coroutine *current;

/// Stack pointer used by the scheduler.
static uintptr_t scheduler_sp;

/// Find a free coroutine slot or return a nonzero error.
static minimk_error_t find_free_coroutine_slot(coroutine **found) noexcept {
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        coroutine *coro = &coroutines[idx];
        if (coro->state != CORO_NULL) {
            continue;
        }
        MINIMK_TRACE("trace: found free coroutine<0x%llx>\n", (unsigned long long)coro);
        *found = coro;
        return 0;
    }
    return MINIMK_EAGAIN;
}

/// Trampoline for starting to execute a coroutine.
static void coro_trampoline(void) noexcept {
    // Ensure that we're in the coroutine world.
    MINIMK_ASSERT(current != nullptr);

    // Transfer control to the coroutine entry, which in turn may
    // voluntarily yield the control to other coroutines.
    current->entry(current->opaque);

    // Mark the coroutine as exited and the scheduler will
    // take care of freeing the allocated resources.
    current->state = CORO_EXITED;
    MINIMK_TRACE("trace: coroutine<0x%llx> EXITED\n", (unsigned long long)current);

    // Transfer the control back to the scheduler.
    minimk_runtime_yield();

    // We should never reach this line.
    MINIMK_ASSERT(false);
}

/// Initialize the given coroutine or return a nonzero error.
static minimk_error_t init_coroutine(coroutine *coro, void (*entry)(void *opaque), void *opaque) noexcept {
    // Zero initialize the whole coroutine
    memset(coro, 0, sizeof(*coro));

    // Initialize the entry
    coro->entry = entry;
    coro->opaque = opaque;

    // Allocate a stack for the coroutine
    minimk_error_t rv = minimk_runtime_stack_alloc(&coro->stack);
    if (rv != 0) {
        return rv;
    }

    // Use assembly to synthesize the stack frame
    minimk_runtime_init_coro_stack(&coro->sp, coro->stack.top, (uintptr_t)coro_trampoline);

    MINIMK_TRACE("trace: coroutine<0x%llx> stack layout:\n", (unsigned long long)coro);
    MINIMK_TRACE("    stack_top: 0x%llx\n", (unsigned long long)coro->stack.top);
    MINIMK_TRACE("    sp: 0x%llx\n", (unsigned long long)coro->sp);

    // Mark as ready to run
    coro->state = CORO_RUNNABLE;
    MINIMK_TRACE("trace: coroutine<0x%llx> RUNNABLE\n", (unsigned long long)coro);
    return 0;
}

/// Free all resources and mark the coroutine slot as empty.
static void destroy_coroutine(coroutine *coro) noexcept {
    (void)minimk_runtime_stack_free(&coro->stack);
    MINIMK_TRACE("trace: coroutine<0x%llx> NULL\n", (unsigned long long)coro);
    memset(coro, 0, sizeof(*coro)); // The zero state implies empty slot
}

minimk_error_t minimk_runtime_go(void (*entry)(void *opaque), void *opaque) noexcept {
    // 1. find an available coroutine slot
    coroutine *coro = nullptr;
    auto rv = find_free_coroutine_slot(&coro);
    if (rv != 0) {
        return rv;
    }

    // 2. initialize the coroutine slot
    if ((rv = init_coroutine(coro, entry, opaque)) != 0) {
        return rv;
    }

    // 3. declare success
    return 0;
}

/// Frees resources used by all the exited coroutines.
static void sched_clean_exited(void) noexcept {
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        auto coro = &coroutines[idx];
        if (coro->state == CORO_EXITED) {
            destroy_coroutine(coro);
            continue;
        }
    }
}

/// Possibly resume the coroutine given the current time and the revents.
static void sched_maybe_resume(coroutine *coro, uint64_t now, short revents) noexcept {
    // Compute whether the coroutine was blocked on a timer and needs to be resumed
    bool resume_timer = (coro->state == CORO_BLOCKED_ON_TIMER && now >= coro->deadline);

    // Compute whether the coroutine was blocked on I/O and needs to be resumed
    (void)revents; // TODO(bassosimone): implement
    bool resume_io = false;

    // Resume the coroutine when needed
    if (resume_timer || resume_io) {
        coro->deadline = 0;
        coro->state = CORO_RUNNABLE;
    }
}

/// Wakeup coroutines whose deadline has expired.
static void sched_expire_deadlines(void) noexcept {
    uint64_t now = minimk_time_monotonic_now();
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        sched_maybe_resume(&coroutines[idx], now, 0);
    }
}

/// Select the first runnable coroutine using a fair algorithm.
static coroutine *sched_pick_runnable(size_t *fair) noexcept {
    // note that wrap is not UB for size_t
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        auto coro = &coroutines[(*fair) % MAX_COROS];
        (*fair)++;
        if (coro->state == CORO_RUNNABLE) {
            return coro;
        }
    }
    return nullptr;
}

/// Ensures that the coroutine stack pointer is within the allocated region
static inline void validate_coro_stack_pointer(const char *context, coroutine *coro) noexcept {
    struct stack *stack = &coro->stack;
    uintptr_t sp = coro->sp;

    MINIMK_TRACE("trace: %s: validating sp=0x%llx in stack[0x%llx, 0x%llx)\n",
                 context,
                 (unsigned long long)sp,
                 (unsigned long long)stack->bottom,
                 (unsigned long long)stack->top);

    MINIMK_ASSERT(sp >= stack->bottom && sp < stack->top);
}

/// Returns the number of coroutines that are not in a null state.
static size_t count_nonnull_coroutines(void) noexcept {
    size_t res = 0;
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        auto coro = &coroutines[idx];
        if (coro->state != CORO_NULL) {
            res++;
            continue;
        }
    }
    return res;
}

/// Attempts to block on the poll system call until a timeout expires or I/O occurs.
///
/// We allow signals to make us return early and recheck the situation. Generally, this
/// library is cooperative and tries to avoid owning the signals.
static void block_on_poll(void) noexcept {
    // 1. pick a reasonable default deadline to avoid blocking for too much time.
    uint64_t now = minimk_time_monotonic_now();
    uint64_t default_timeout = 100000000;
    uint64_t deadline = minimk_integer_u64_satadd(now, default_timeout);

    MINIMK_TRACE("trace: poll initial deadline=%llu [us]\n", (unsigned long long)deadline);

    // 2. we need to poll at most a socket per coroutine.
    //
    // On linux/amd64 each structure is 8 byte and we have 16 coroutines
    // which causes the stack to grow by 128 bytes.
    minimk_pollfd fds[MAX_COROS] = {};
    size_t numfds = 0;

    // 3. scan the coroutines list and init deadline and fds.
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        auto coro = &coroutines[idx];

        if (coro->state == CORO_BLOCKED_ON_TIMER) {
            deadline = (coro->deadline < deadline) ? coro->deadline : deadline;
            continue;
        }

        // TODO(bassosimone): operate on sockets when we
        // implement this functionality
    }

    MINIMK_TRACE("trace: poll adjusted deadline=%llu [us]\n", (unsigned long long)deadline);

    // 4. compute the poll timeout.
    uint64_t poll_timeout64 = (((deadline > now) ? (deadline - now) : 0) / 1000000) + 1;
    int poll_timeout = (int)((poll_timeout64) < INT_MAX ? poll_timeout64 : INT_MAX);

    MINIMK_TRACE("trace: poll_timeout64=%llu [ms]\n", (unsigned long long)poll_timeout64);
    MINIMK_TRACE("trace: poll_timeout=%lld [ms]\n", (long long)poll_timeout);

    // 5. invoke the poll system call and handle is result.
    //
    // Note that under Linux poll fails in these cases:
    //
    // - EFAULT fds points outside the process's accessible address space.
    //
    // - EINTR  A signal occurred before any requested event.
    //
    // - EINVAL The nfds value exceeds the RLIMIT_NOFILE value.
    //
    // - ENOMEM Unable to allocate memory for kernel data structures.
    MINIMK_TRACE("trace: poll fds=0x%llx numfds=%llu timeout=%lld\n",
                 (unsigned long long)fds,
                 (unsigned long long)numfds,
                 (unsigned long)poll_timeout);

    size_t active = 0;
    auto rc = minimk_poll(fds, numfds, poll_timeout, &active);

    MINIMK_TRACE("trace: poll rc=%llu active=%llu\n", (unsigned long long)rc, (unsigned long long)active);

    if (rc != 0) {
        return;
    }

    // 6. check what we need to resume now
    now = minimk_time_monotonic_now();
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        sched_maybe_resume(&coroutines[idx], now, fds[idx].revents);
    }
}

void minimk_runtime_run(void) noexcept {
    // Ensure we are not yet inside the coroutine world.
    MINIMK_ASSERT(current == nullptr);

    // Continue until we're out of coroutines.
    for (size_t fair = 0; count_nonnull_coroutines() > 0;) {
        // Check whether there are coroutines that need cleanup.
        sched_clean_exited();

        // Wakeup coroutines whose deadline has expired.
        sched_expire_deadlines();

        // Fairly select the first runnable coroutine.
        current = sched_pick_runnable(&fair);

        // If there are no runnable coroutines, wait for something to happen.
        if (current == nullptr) {
            block_on_poll();
            continue;
        }

        // Log the state before switching
        MINIMK_TRACE("trace: scheduler: switching to coroutine<0x%llx> sp=%llx\n",
                     (unsigned long long)current,
                     (unsigned long long)current->sp);

        validate_coro_stack_pointer("before_switch", current);

        MINIMK_TRACE("trace: scheduler_sp before switch: 0x%llx\n", (unsigned long long)scheduler_sp);

        // Perform the actual switching
        minimk_runtime_switch(&scheduler_sp, current->sp);

        // Log the state after switching
        MINIMK_TRACE("trace: scheduler: returned from coroutine<0x%llx> sp=%llx\n",
                     (unsigned long long)current,
                     (unsigned long long)scheduler_sp);

        MINIMK_TRACE("trace: scheduler_sp after switch: 0x%llx\n", (unsigned long long)scheduler_sp);

        validate_coro_stack_pointer("after_switch", current);

        // We're now inside the scheduler again.
        current = nullptr;
    }
}

void minimk_runtime_yield(void) noexcept {
    // Ensure we're inside the coroutine world.
    MINIMK_ASSERT(current != nullptr);

    // Manually switch back to the scheduler stack.
    minimk_runtime_switch(&current->sp, scheduler_sp);
}

void minimk_runtime_nanosleep(uint64_t nanosec) noexcept {
    // Ensure we're inside the coroutine world.
    MINIMK_ASSERT(current != nullptr);

    // Get the current monotonic clock reading
    uint64_t deadline = minimk_time_monotonic_now();
    deadline = minimk_integer_u64_satadd(deadline, nanosec);

    // Suspend the coroutine
    current->state = CORO_BLOCKED_ON_TIMER;
    current->deadline = deadline;
    minimk_runtime_yield();
}
