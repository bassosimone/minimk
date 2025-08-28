// File: libminimk/runtime/runtime.cpp
// Purpose: cooperative coroutine runtime implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/runtime.h> // for minimk_runtime_run

#include <stddef.h> // for size_t
#include <stdint.h> // for uintptr_t
#include <string.h> // for memset

#include "assert.h" // for MINIMK_ASSERT
#include "stack.h"  // for struct stack
#include "switch.h" // for minimk_switch
#include "trace.h"  // for MINIMK_TRACE

/// The coroutine slot is unused.
#define CORO_NULL 0

/// The coroutine can run immediately.
#define CORO_RUNNABLE 1

/// The coroutine has finished.
#define CORO_EXITED 2

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
    //
    // If we're not aligned, this may backfire on arm64
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

void minimk_runtime_run(void) noexcept {
    // Ensure we are not yet inside the coroutine world.
    MINIMK_ASSERT(current == nullptr);

    // Continue until we're out of coroutines.
    for (size_t fair = 0;;) {
        // Check whether there are coroutines that need cleanup.
        sched_clean_exited();

        // Fairly select the first runnable coroutine.
        current = sched_pick_runnable(&fair);

        // If there are no runnable coroutines, we're outta here.
        if (current == nullptr) {
            break;
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
