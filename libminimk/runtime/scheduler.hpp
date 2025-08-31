// File: libminimk/runtime/scheduler.hpp
// Purpose: coroutine scheduler
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_SCHEDULER_HPP
#define LIBMINIMK_RUNTIME_SCHEDULER_HPP

#include "../integer/u64.h" // for minimk_integer_u64_satadd

#include "coroutine.h" // for struct coroutine
#include "scheduler.h" // for struct scheduler
#include "switch.h"    // for minimk_switch

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/time.h>    // for minimk_time_monotonic_now
#include <minimk/trace.h>   // for MINIMK_TRACE

#include <limits.h> // for INT_MAX
#include <stddef.h> // for size_t
#include <stdint.h> // for uintptr_t

static inline struct coroutine *minimk_runtime_scheduler_get_coroutine_slot_impl( //
        struct scheduler *sched, size_t idx) noexcept {
    MINIMK_ASSERT(idx >= 0 && idx < MAX_COROS);
#pragma clang unsafe_buffer_usage begin
    return &sched->coroutines[idx];
#pragma clang unsafe_buffer_usage end
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
        MINIMK_TRACE("trace: found free coroutine<0x%llx>\n",
                     reinterpret_cast<unsigned long long>(coro));
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
          decltype(minimk_runtime_coroutine_maybe_resume) M_resume =
                  minimk_runtime_coroutine_maybe_resume>
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
          decltype(minimk_runtime_coroutine_maybe_resume) M_resume =
                  minimk_runtime_coroutine_maybe_resume>
void minimk_runtime_scheduler_block_on_poll_impl(struct scheduler *sched) noexcept {
    // 1. pick a reasonable default deadline to avoid blocking for too much time.
    uint64_t now = minimk_time_monotonic_now();
    uint64_t default_timeout = 10000000000;
    uint64_t deadline = minimk_integer_u64_satadd(now, default_timeout);

    MINIMK_TRACE("trace: poll initial deadline=%llu [us]\n",
                 static_cast<unsigned long long>(deadline));

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

#pragma clang unsafe_buffer_usage begin
            auto fd = &fds[idx];
#pragma clang unsafe_buffer_usage end

            fd->events = coro->events;
            fd->fd = coro->sock;

            MINIMK_TRACE("trace: poll fd=%llu events=%llu\n",
                         static_cast<unsigned long long>(coro->sock),
                         static_cast<unsigned long long>(coro->events));
            continue;
        }
    }

    MINIMK_TRACE("trace: poll adjusted deadline=%llu [us]\n",
                 static_cast<unsigned long long>(deadline));

    // 4. compute the poll timeout.
    uint64_t poll_timeout64 = (((deadline > now) ? (deadline - now) : 0) / 1000000) + 1;
    int poll_timeout = static_cast<int>((poll_timeout64) < INT_MAX ? poll_timeout64 : INT_MAX);

    MINIMK_TRACE("trace: poll_timeout64=%llu [ms]\n",
                 static_cast<unsigned long long>(poll_timeout64));
    MINIMK_TRACE("trace: poll_timeout=%lld [ms]\n", static_cast<long long>(poll_timeout));

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
                 reinterpret_cast<unsigned long long>(fds), static_cast<unsigned long long>(numfds),
                 static_cast<long long>(poll_timeout));

    size_t active = 0;
    auto poll_rc = M_poll(fds, numfds, poll_timeout, &active);

    MINIMK_TRACE("trace: poll rc=%llu active=%llu\n", static_cast<unsigned long long>(poll_rc),
                 static_cast<unsigned long long>(active));

    if (poll_rc == MINIMK_EINTR) {
        return;
    }
    MINIMK_ASSERT(poll_rc == 0);

    // 6. check what we need to resume now
    now = minimk_time_monotonic_now();
    for (size_t idx = 0; idx < MAX_COROS; idx++) {

#pragma clang unsafe_buffer_usage begin
        auto fd = &fds[idx];
#pragma clang unsafe_buffer_usage end

        M_resume(M_get(sched, idx), now, fd->revents);
    }
}

/// Utility function to factor code for switching coroutine.
template <decltype(minimk_runtime_switch) M_switch = minimk_runtime_switch,
          decltype(minimk_runtime_coroutine_validate_stack_pointer) M_validate =
                  minimk_runtime_coroutine_validate_stack_pointer>
void minimk_runtime_scheduler_switch_impl(struct scheduler *sched) noexcept {
    // Log the state before switching
    MINIMK_TRACE("trace: scheduler: switching to coroutine<0x%llx> sp=%llx\n",
                 reinterpret_cast<unsigned long long>(sched->current),
                 static_cast<unsigned long long>(sched->current->sp));

    M_validate("before_switch", sched->current);

    MINIMK_TRACE("trace: scheduler_sp before switch: 0x%llx\n",
                 static_cast<unsigned long long>(sched->sp));

    // Perform the actual switching
    M_switch(&sched->sp, sched->current->sp);

    // Log the state after switching
    MINIMK_TRACE("trace: scheduler: returned from coroutine<0x%llx> sp=%llx\n",
                 reinterpret_cast<unsigned long long>(sched->current),
                 static_cast<unsigned long long>(sched->sp));

    MINIMK_TRACE("trace: scheduler_sp after switch: 0x%llx\n",
                 static_cast<unsigned long long>(sched->sp));

    M_validate("after_switch", sched->current);
}

#endif // LIBMINIMK_RUNTIME_SCHEDULER_HPP
