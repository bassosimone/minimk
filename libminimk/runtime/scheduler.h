// File: libminimk/runtime/scheduler.h
// Purpose: coroutine scheduler
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_SCHEDULER_H
#define LIBMINIMK_RUNTIME_SCHEDULER_H

#include "coroutine.h" // for struct coroutine

#include <minimk/cdefs.h>   // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_socket_t

#include <stddef.h> // for size_t
#include <stdint.h> // for uintptr_t

/// Maximum number of coroutines we can create.
#define MAX_COROS 16

/// Coroutine scheduler.
struct scheduler {
    /// Slots for coroutines we manage.
    struct coroutine coroutines[MAX_COROS];

    /// Pointer to currently running coroutine.
    struct coroutine *current;

    /// Stack pointer used by the scheduler.
    uintptr_t sp;
};

MINIMK_BEGIN_DECLS

/// Returns the coroutine slot corresponding to the given index or panics.
struct coroutine *minimk_runtime_scheduler_get_coroutine_slot(struct scheduler *sched,
                                                              size_t idx) MINIMK_NOEXCEPT;

/// Find a free coroutine slot or return a nonzero error.
minimk_error_t minimk_runtime_scheduler_find_free_coroutine_slot( //
        struct scheduler *sched, struct coroutine **found) MINIMK_NOEXCEPT;

/// Frees resources used by all the exited coroutines.
void minimk_runtime_scheduler_clean_exited_coroutines(struct scheduler *sched) MINIMK_NOEXCEPT;

/// Wakeup coroutines whose deadline has expired.
void minimk_runtime_scheduler_maybe_expire_deadlines(struct scheduler *sched) MINIMK_NOEXCEPT;

/// Select the first runnable coroutine using a fair algorithm.
struct coroutine *minimk_runtime_scheduler_pick_runnable( //
        struct scheduler *sched, size_t *fair) MINIMK_NOEXCEPT;

/// Returns the number of coroutines that are not in a null state.
size_t minimk_runtime_scheduler_count_nonnull_coroutines(struct scheduler *sched) MINIMK_NOEXCEPT;

/// Attempts to block on the poll system call until a timeout expires or I/O occurs.
///
/// We allow signals to make us return early and recheck the situation. Generally, this
/// library is cooperative and tries to avoid owning the signals.
void minimk_runtime_scheduler_block_on_poll(struct scheduler *sched) MINIMK_NOEXCEPT;

/// Utility function to factor code for switching coroutine.
void minimk_runtime_scheduler_switch(struct scheduler *sched) MINIMK_NOEXCEPT;

/// Main coroutine function called by the assembly trampoline.
void minimk_runtime_scheduler_coroutine_main(struct scheduler *sched) MINIMK_NOEXCEPT;

/// Creates a runnable coroutine within the scheduler.
minimk_error_t minimk_runtime_scheduler_coroutine_create( //
        struct scheduler *sched, void (*entry)(void *opaque), void *opaque) MINIMK_NOEXCEPT;

/// Runs the scheduler until no coroutines remain.
void minimk_runtime_scheduler_run(struct scheduler *sched) MINIMK_NOEXCEPT;

/// Yields the CPU from current to another coroutine.
void minimk_runtime_scheduler_coroutine_yield(struct scheduler *sched) MINIMK_NOEXCEPT;

/// Suspends current coroutine until the given timeout expires.
void minimk_runtime_scheduler_coroutine_suspend_timer(struct scheduler *sched,
                                                      uint64_t nanosec) MINIMK_NOEXCEPT;

/// Suspends current coroutine waiting for I/O with timeout.
minimk_error_t minimk_runtime_scheduler_coroutine_suspend_io( //
        struct scheduler *sched, minimk_syscall_socket_t sock, short events,
        uint64_t nanosec) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_RUNTIME_SCHEDULER_H
