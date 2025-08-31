// File: libminimk/runtime/runtime.cpp
// Purpose: cooperative coroutine runtime implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../integer/u64.h" // for minimk_integer_u64_satadd

#include "coroutine.h" // for struct coroutine
#include "scheduler.h" // for struct scheduler
#include "switch.h"    // for minimk_switch

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/cdefs.h>   // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/runtime.h> // for minimk_runtime_run
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/time.h>    // for minimk_time_monotonic_now

#include <stddef.h> // for size_t
#include <stdint.h> // for uintptr_t

/// Global scheduler to use
static scheduler s0;

/// Trampoline for starting to execute a coroutine.
static void coro_trampoline(void) noexcept __attribute__((noreturn));

static void coro_trampoline(void) noexcept {
    // Ensure that we're in the coroutine world.
    MINIMK_ASSERT(s0.current != nullptr);

    // Transfer control to the coroutine entry, which in turn may
    // voluntarily yield the control to other coroutines.
    s0.current->entry(s0.current->opaque);

    // Mark the coroutine as exited and the scheduler will
    // take care of freeing the allocated resources.
    minimk_runtime_coroutine_mark_as_exited(s0.current);

    // Transfer the control back to the scheduler.
    minimk_runtime_yield();

    // We should never reach this line.
    MINIMK_ASSERT(false);
}

/// Function that creates a runnable coroutine within the scheduler.
static inline minimk_error_t minimk_runtime_scheduler_coroutine_create( //
        scheduler *sched, void (*entry)(void *opaque), void *opaque) noexcept {
    // 1. find an available coroutine slot
    coroutine *coro = nullptr;
    auto rv = minimk_runtime_scheduler_find_free_coroutine_slot(sched, &coro);
    if (rv != 0) {
        return rv;
    }

    // 2. initialize the coroutine slot
    if ((rv = minimk_runtime_coroutine_init(coro, coro_trampoline, entry, opaque)) != 0) {
        return rv;
    }

    // 3. declare success
    return 0;
}

minimk_error_t minimk_runtime_go(void (*entry)(void *opaque), void *opaque) noexcept {
    return minimk_runtime_scheduler_coroutine_create(&s0, entry, opaque);
}

/// Function that runs the given scheduler until we're out of coroutines to run.
static inline void minimk_runtime_scheduler_run(scheduler *sched) noexcept {
    // Ensure we are not yet inside the coroutine world.
    MINIMK_ASSERT(sched->current == nullptr);

    // Continue until we're out of coroutines.
    for (size_t fair = 0; minimk_runtime_scheduler_count_nonnull_coroutines(sched) > 0;) {
        // Check whether there are coroutines that need cleanup.
        minimk_runtime_scheduler_clean_exited_coroutines(sched);

        // Wakeup coroutines whose deadline has expired.
        minimk_runtime_scheduler_maybe_expire_deadlines(sched);

        // Fairly select the first runnable coroutine.
        sched->current = minimk_runtime_scheduler_pick_runnable(sched, &fair);

        // If there are no runnable coroutines, wait for something to
        // happen but avoid sleeping if everyine is dead.
        if (sched->current == nullptr) {
            if (minimk_runtime_scheduler_count_nonnull_coroutines(sched) > 0) {
                minimk_runtime_scheduler_block_on_poll(sched);
            }
            continue;
        }

        // Transfer the control to the current coroutine
        minimk_runtime_scheduler_switch(sched);

        // We're now inside the scheduler again.
        sched->current = nullptr;
    }
}

void minimk_runtime_run(void) noexcept {
    minimk_runtime_scheduler_run(&s0);
}

/// Function to yield the CPU from current to another coroutine.
static inline void minimk_runtime_scheduler_coroutine_yield(scheduler *sched) noexcept {
    // Ensure we're inside the coroutine world.
    MINIMK_ASSERT(sched->current != nullptr);

    // Manually switch back to the scheduler stack.
    minimk_runtime_switch(&sched->current->sp, sched->sp);
}

void minimk_runtime_yield(void) noexcept {
    minimk_runtime_scheduler_coroutine_yield(&s0);
}

/// Function that suspends the current coroutine until the given timeout expires.
static inline void //
minimk_runtime_scheduler_coroutine_suspend_timer(scheduler *sched, uint64_t nanosec) noexcept {
    // Ensure we're inside the coroutine world.
    MINIMK_ASSERT(sched->current != nullptr);

    // Get the current monotonic clock reading
    uint64_t deadline = minimk_time_monotonic_now();
    deadline = minimk_integer_u64_satadd(deadline, nanosec);

    // Suspend
    minimk_runtime_coroutine_suspend_timer(sched->current, deadline);

    // Schedule
    minimk_runtime_scheduler_coroutine_yield(sched);

    // Resume
    minimk_runtime_coroutine_resume_timer(sched->current);
}

void minimk_runtime_nanosleep(uint64_t nanosec) noexcept {
    minimk_runtime_scheduler_coroutine_suspend_timer(&s0, nanosec);
}

/// Internal function to uniformly handle suspending for I/O.
static minimk_error_t minimk_runtime_scheduler_coroutine_suspend_io( //
        scheduler *sched, minimk_syscall_socket_t sock, short events, uint64_t nanosec) noexcept {
    // Ensure we're inside the coroutine world.
    MINIMK_ASSERT(sched->current != nullptr);

    // Actually suspend the coroutine
    minimk_runtime_coroutine_suspend_io(sched->current, sock, events, nanosec);

    // Schedule
    minimk_runtime_scheduler_coroutine_yield(sched);

    // Resume the coroutine and mark runnable again
    return minimk_runtime_coroutine_resume_io(sched->current, sock, events);
}

minimk_error_t minimk_runtime_suspend_read(minimk_syscall_socket_t sock,
                                           uint64_t nanosec) MINIMK_NOEXCEPT {
    return minimk_runtime_scheduler_coroutine_suspend_io(&s0, sock, minimk_syscall_pollin, nanosec);
}

minimk_error_t minimk_runtime_suspend_write(minimk_syscall_socket_t sock,
                                            uint64_t nanosec) MINIMK_NOEXCEPT {
    return minimk_runtime_scheduler_coroutine_suspend_io(&s0, sock, minimk_syscall_pollout,
                                                         nanosec);
}
