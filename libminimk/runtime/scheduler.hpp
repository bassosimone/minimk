// File: libminimk/runtime/scheduler.hpp
// Purpose: coroutine scheduler
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_SCHEDULER_HPP
#define LIBMINIMK_RUNTIME_SCHEDULER_HPP

#include "../cast/static.hpp" // for CAST_ULL
#include "../integer/u64.h"   // for minimk_integer_u64_satadd

#include "coroutine.h" // for struct coroutine
#include "scheduler.h" // for struct scheduler
#include "switch.h"    // for minimk_switch

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/cdefs.h>   // for MINIMK_UNSAFE_BUFFER_USAGE_*
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/time.h>    // for minimk_time_monotonic_now
#include <minimk/trace.h>   // for MINIMK_TRACE_SCHEDULER

#include <limits.h> // for INT_MAX
#include <stddef.h> // for size_t
#include <stdint.h> // for uintptr_t

static inline struct coroutine *minimk_runtime_scheduler_get_coroutine_slot_impl( //
        struct scheduler *sched, size_t idx) noexcept {
    MINIMK_ASSERT(idx >= 0 && idx < MAX_COROS);
    MINIMK_UNSAFE_BUFFER_USAGE_BEGIN
    return &sched->coroutines[idx];
    MINIMK_UNSAFE_BUFFER_USAGE_END
}

template <decltype(minimk_runtime_scheduler_get_coroutine_slot) M_get =
                  minimk_runtime_scheduler_get_coroutine_slot>
minimk_error_t minimk_runtime_scheduler_find_free_coroutine_slot_impl( //
        struct scheduler *sched, struct coroutine **found) noexcept {
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        coroutine *coro = M_get(sched, idx);
        if (coro->state != CORO_NULL) {
            continue;
        }
        MINIMK_TRACE_SCHEDULER("%p found slot=%p\n", CAST_VOID_P(sched), CAST_VOID_P(coro));
        *found = coro;
        return 0;
    }
    return MINIMK_EAGAIN;
}

template <decltype(minimk_runtime_scheduler_get_coroutine_slot) M_get =
                  minimk_runtime_scheduler_get_coroutine_slot,
          decltype(minimk_runtime_coroutine_finish) M_finish = minimk_runtime_coroutine_finish>
void minimk_runtime_scheduler_clean_exited_coroutines_impl(struct scheduler *sched) noexcept {
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        coroutine *coro = M_get(sched, idx);
        if (coro->state == CORO_EXITED) {
            M_finish(coro);
            continue;
        }
    }
}

template <decltype(minimk_runtime_scheduler_get_coroutine_slot) M_get =
                  minimk_runtime_scheduler_get_coroutine_slot,
          decltype(minimk_runtime_coroutine_maybe_resume) M_resume = minimk_runtime_coroutine_maybe_resume>
void minimk_runtime_scheduler_maybe_expire_deadlines_impl(struct scheduler *sched) noexcept {
    uint64_t now = minimk_time_monotonic_now();
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        M_resume(M_get(sched, idx), now, 0);
    }
}

template <decltype(minimk_runtime_scheduler_get_coroutine_slot) M_get =
                  minimk_runtime_scheduler_get_coroutine_slot>
struct coroutine *minimk_runtime_scheduler_pick_runnable_impl( //
        struct scheduler *sched, size_t *fair) noexcept {
    // note that wrap is not UB for size_t
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        coroutine *coro = M_get(sched, (*fair) % MAX_COROS);
        (*fair)++;
        if (coro->state == CORO_RUNNABLE) {
            return coro;
        }
    }
    return nullptr;
}

template <decltype(minimk_runtime_scheduler_get_coroutine_slot) M_get =
                  minimk_runtime_scheduler_get_coroutine_slot>
size_t minimk_runtime_scheduler_count_nonnull_coroutines_impl(struct scheduler *sched) noexcept {
    size_t res = 0;
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        coroutine *coro = M_get(sched, idx);
        if (coro->state != CORO_NULL) {
            res++;
            continue;
        }
    }
    return res;
}

template <decltype(minimk_runtime_scheduler_get_coroutine_slot) M_get =
                  minimk_runtime_scheduler_get_coroutine_slot,
          decltype(minimk_syscall_poll) M_poll = minimk_syscall_poll,
          decltype(minimk_runtime_coroutine_maybe_resume) M_resume = minimk_runtime_coroutine_maybe_resume>
void minimk_runtime_scheduler_block_on_poll_impl(struct scheduler *sched) noexcept {
    // 1. pick a reasonable default deadline to avoid blocking for too much time.
    uint64_t now = minimk_time_monotonic_now();
    uint64_t default_timeout = 10000000000;
    uint64_t deadline = minimk_integer_u64_satadd(now, default_timeout);

    MINIMK_TRACE_SCHEDULER("%p poll\n", CAST_VOID_P(sched));
    MINIMK_TRACE_SCHEDULER("%p    initial deadline=%llu [us]\n", CAST_VOID_P(sched), CAST_ULL(deadline));

    // 2. we need to poll at most a socket per coroutine.
    //
    // On linux/amd64 each structure is 8 byte and we have 16 coroutines
    // which causes the stack to grow by 128 bytes.
    minimk_syscall_pollfd_t fds[MAX_COROS] = {};
    size_t numfds = MAX_COROS;

    // 3. scan the coroutines list and init deadline and fds.
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        auto coro = M_get(sched, idx);

        // 3.1. arm timers
        if (coro->state == CORO_BLOCKED_ON_TIMER) {
            deadline = (coro->deadline < deadline) ? coro->deadline : deadline;
            continue;
        }

        // 3.2. deal with I/O
        if (coro->state == CORO_BLOCKED_ON_IO) {
            deadline = (coro->deadline < deadline) ? coro->deadline : deadline;

            MINIMK_ASSERT(coro->sock != minimk_syscall_invalid_socket);
            MINIMK_ASSERT(coro->events != 0);
            MINIMK_ASSERT(coro->revents == 0);

            MINIMK_UNSAFE_BUFFER_USAGE_BEGIN
            auto fd = &fds[idx];
            MINIMK_UNSAFE_BUFFER_USAGE_END

            fd->events = coro->events;
            fd->fd = coro->sock;

            MINIMK_TRACE_SCHEDULER("%p      fd=%llu\n", CAST_VOID_P(sched), CAST_ULL(coro->sock));
            MINIMK_TRACE_SCHEDULER("%p        events=%llu\n", CAST_VOID_P(sched), CAST_ULL(coro->events));
            continue;
        }
    }

    MINIMK_TRACE_SCHEDULER("%p    adjusted deadline=%llu [us]\n", CAST_VOID_P(sched), CAST_ULL(deadline));

    // 4. compute the poll timeout.
    uint64_t poll_timeout64 = (((deadline > now) ? (deadline - now) : 0) / 1000000) + 1;
    int poll_timeout = static_cast<int>((poll_timeout64) < INT_MAX ? poll_timeout64 : INT_MAX);

    MINIMK_TRACE_SCHEDULER("%p    timeout64=%llu [ms]\n", CAST_VOID_P(sched), CAST_ULL(poll_timeout64));
    MINIMK_TRACE_SCHEDULER("%p    timeout=%llu [ms]\n", CAST_VOID_P(sched), CAST_ULL(poll_timeout));

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
    MINIMK_TRACE_SCHEDULER("%p    numfds=%llu\n", CAST_VOID_P(sched), CAST_ULL(numfds));

    size_t active = 0;
    auto poll_rc = M_poll(fds, numfds, poll_timeout, &active);

    MINIMK_TRACE_SCHEDULER("%p    rc=%llu\n", CAST_VOID_P(sched), CAST_ULL(poll_rc));
    MINIMK_TRACE_SCHEDULER("%p    active=%llu\n", CAST_VOID_P(sched), CAST_ULL(active));

    if (poll_rc == MINIMK_EINTR) {
        return;
    }
    MINIMK_ASSERT(poll_rc == 0);

    // 6. check what we need to resume now
    now = minimk_time_monotonic_now();
    for (size_t idx = 0; idx < MAX_COROS; idx++) {

        MINIMK_UNSAFE_BUFFER_USAGE_BEGIN
        auto fd = &fds[idx];
        MINIMK_UNSAFE_BUFFER_USAGE_END

        M_resume(M_get(sched, idx), now, fd->revents);
    }
}

/// Utility function to factor code for switching coroutine.
template <decltype(minimk_runtime_switch) M_switch = minimk_runtime_switch,
          decltype(minimk_runtime_coroutine_validate_stack_pointer) M_validate =
                  minimk_runtime_coroutine_validate_stack_pointer>
void minimk_runtime_scheduler_switch_impl(struct scheduler *sched) noexcept {
    // Log the state before switching
    MINIMK_TRACE_SCHEDULER("%p switching to %p\n", CAST_VOID_P(sched), CAST_VOID_P(sched->current));
    MINIMK_TRACE_SCHEDULER("%p    coro_sp=0x%llu\n", CAST_VOID_P(sched), CAST_ULL(sched->current->sp));
    MINIMK_TRACE_SCHEDULER("%p    sched_sp=0x%llu\n", CAST_VOID_P(sched), CAST_ULL(sched->sp));

    M_validate("before_switch", sched->current);

    // Perform the actual switching
    M_switch(&sched->sp, sched->current->sp);

    // Log the state after switching
    MINIMK_TRACE_SCHEDULER("%p returned from %p\n", CAST_VOID_P(sched), CAST_VOID_P(sched->current));
    MINIMK_TRACE_SCHEDULER("%p    coro_sp=0x%llu\n", CAST_VOID_P(sched), CAST_ULL(sched->current->sp));
    MINIMK_TRACE_SCHEDULER("%p    sched_sp=0x%llu\n", CAST_VOID_P(sched), CAST_ULL(sched->sp));

    M_validate("after_switch", sched->current);
}

template <
        decltype(minimk_runtime_coroutine_mark_as_exited) M_mark_exited =
                minimk_runtime_coroutine_mark_as_exited,
        decltype(minimk_runtime_scheduler_coroutine_yield) M_yield = minimk_runtime_scheduler_coroutine_yield>
void minimk_runtime_scheduler_coroutine_main_impl(struct scheduler *sched) noexcept {
    // Ensure that we're in the coroutine world.
    MINIMK_ASSERT(sched->current != nullptr);

    // Transfer control to the coroutine entry, which in turn may
    // voluntarily yield the control to other coroutines.
    sched->current->entry(sched->current->opaque);

    // Mark the coroutine as exited and the scheduler will
    // take care of freeing the allocated resources.
    M_mark_exited(sched->current);

    // Transfer the control back to the scheduler.
    M_yield(sched);

    // We should never reach this line.
    MINIMK_ASSERT(false);
}

template <decltype(minimk_runtime_scheduler_find_free_coroutine_slot) M_find_slot =
                  minimk_runtime_scheduler_find_free_coroutine_slot,
          decltype(minimk_runtime_coroutine_init) M_init = minimk_runtime_coroutine_init,
          decltype(minimk_runtime_asm_trampoline) M_trampoline = minimk_runtime_asm_trampoline>
minimk_error_t minimk_runtime_scheduler_coroutine_create_impl( //
        struct scheduler *sched, void (*entry)(void *opaque), void *opaque) noexcept {
    // 1. find an available coroutine slot
    coroutine *coro = nullptr;
    auto rv = M_find_slot(sched, &coro);
    if (rv != 0) {
        return rv;
    }

    // 2. initialize the coroutine slot
    rv = M_init(coro, M_trampoline, sched, entry, opaque);
    if (rv != 0) {
        return rv;
    }

    // 3. declare success
    return 0;
}

template <decltype(minimk_runtime_scheduler_count_nonnull_coroutines) M_count =
                  minimk_runtime_scheduler_count_nonnull_coroutines,
          decltype(minimk_runtime_scheduler_clean_exited_coroutines) M_clean =
                  minimk_runtime_scheduler_clean_exited_coroutines,
          decltype(minimk_runtime_scheduler_maybe_expire_deadlines) M_expire =
                  minimk_runtime_scheduler_maybe_expire_deadlines,
          decltype(minimk_runtime_scheduler_pick_runnable) M_pick = minimk_runtime_scheduler_pick_runnable,
          decltype(minimk_runtime_scheduler_block_on_poll) M_poll = minimk_runtime_scheduler_block_on_poll,
          decltype(minimk_runtime_scheduler_switch) M_switch = minimk_runtime_scheduler_switch>
void minimk_runtime_scheduler_run_impl(struct scheduler *sched) noexcept {
    // Ensure we are not yet inside the coroutine world.
    MINIMK_ASSERT(sched->current == nullptr);

    // Continue until we're out of coroutines.
    for (size_t fair = 0; M_count(sched) > 0;) {
        MINIMK_TRACE_SCHEDULER("%p loop\n", CAST_VOID_P(sched));

        // Check whether there are coroutines that need cleanup.
        M_clean(sched);

        // Wakeup coroutines whose deadline has expired.
        M_expire(sched);

        // Fairly select the first runnable coroutine.
        sched->current = M_pick(sched, &fair);

        // If there are no runnable coroutines, wait for something to
        // happen but avoid sleeping if everyone is dead.
        if (sched->current == nullptr) {
            if (M_count(sched) > 0) {
                M_poll(sched);
            }
            continue;
        }

        // Transfer the control to the current coroutine
        M_switch(sched);

        // We're now inside the scheduler again.
        sched->current = nullptr;
    }
}

template <decltype(minimk_runtime_switch) M_switch = minimk_runtime_switch>
void minimk_runtime_scheduler_coroutine_yield_impl(struct scheduler *sched) noexcept {
    // Ensure we're inside the coroutine world.
    MINIMK_ASSERT(sched->current != nullptr);

    // Manually switch back to the scheduler stack.
    M_switch(&sched->current->sp, sched->sp);
}

template <
        decltype(minimk_time_monotonic_now) M_now = minimk_time_monotonic_now,
        decltype(minimk_integer_u64_satadd) M_add = minimk_integer_u64_satadd,
        decltype(minimk_runtime_coroutine_suspend_timer) M_suspend = minimk_runtime_coroutine_suspend_timer,
        decltype(minimk_runtime_scheduler_coroutine_yield) M_yield = minimk_runtime_scheduler_coroutine_yield,
        decltype(minimk_runtime_coroutine_resume_timer) M_resume = minimk_runtime_coroutine_resume_timer>
void minimk_runtime_scheduler_coroutine_suspend_timer_impl(struct scheduler *sched,
                                                           uint64_t nanosec) noexcept {
    // Ensure we're inside the coroutine world.
    MINIMK_ASSERT(sched->current != nullptr);

    // Get the current monotonic clock reading
    uint64_t deadline = M_now();
    deadline = M_add(deadline, nanosec);

    // Suspend
    M_suspend(sched->current, deadline);

    // Schedule
    M_yield(sched);

    // Resume
    M_resume(sched->current);
}

template <
        decltype(minimk_runtime_coroutine_suspend_io) M_suspend = minimk_runtime_coroutine_suspend_io,
        decltype(minimk_runtime_scheduler_coroutine_yield) M_yield = minimk_runtime_scheduler_coroutine_yield,
        decltype(minimk_runtime_coroutine_resume_io) M_resume = minimk_runtime_coroutine_resume_io>
minimk_error_t minimk_runtime_scheduler_coroutine_suspend_io_impl( //
        struct scheduler *sched, minimk_syscall_socket_t sock, short events, uint64_t nanosec) noexcept {
    // Ensure we're inside the coroutine world.
    MINIMK_ASSERT(sched->current != nullptr);

    // Actually suspend the coroutine
    M_suspend(sched->current, sock, events, nanosec);

    // Schedule
    M_yield(sched);

    // Resume the coroutine and mark runnable again
    return M_resume(sched->current, sock, events);
}

#endif // LIBMINIMK_RUNTIME_SCHEDULER_HPP
