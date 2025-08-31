// File: libminimk/runtime/coroutine.hpp
// Purpose: coroutine specific code
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_COROUTINE_HPP
#define LIBMINIMK_RUNTIME_COROUTINE_HPP

#include "../cast/static.hpp" // for CAST_VOID_P
#include "../integer/u64.h"   // for minimk_integer_u64_satadd

#include "coroutine.h" // for struct coroutine
#include "stack.h"     // for minimk_runtime_stack_alloc
#include "switch.h"    // for minimk_runtime_init_coro_stack

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_invalid_socket
#include <minimk/time.h>    // for minimk_time_monotonic_now
#include <minimk/trace.h>   // for MINIMK_TRACE_COROUTINE

#include <stdint.h> // for int64_t

/// Testable implementation of minimk_runtime_coroutine_init.
template <decltype(minimk_runtime_stack_alloc) M_stack_alloc = minimk_runtime_stack_alloc,
          decltype(minimk_runtime_init_coro_stack) M_init_coro_stack = minimk_runtime_init_coro_stack>
minimk_error_t minimk_runtime_coroutine_init_impl(struct coroutine *coro, void (*trampoline)(void),
                                                  struct scheduler *sched, void (*entry)(void *opaque),
                                                  void *opaque) noexcept {
    // Zero initialize the whole coroutine
    *coro = {};

    // Initialize the entry
    MINIMK_TRACE_COROUTINE("%p init\n", CAST_VOID_P(coro));
    coro->sock = minimk_syscall_invalid_socket;
    coro->entry = entry;
    coro->opaque = opaque;

    // Allocate a stack for the coroutine
    minimk_error_t rv = M_stack_alloc(&coro->stack);
    if (rv != 0) {
        return rv;
    }

    MINIMK_TRACE_COROUTINE("%p alloc_stack\n", CAST_VOID_P(coro));
    MINIMK_TRACE_COROUTINE("%p    base=0x%llx\n", CAST_VOID_P(coro), CAST_ULL(coro->stack.base));
    MINIMK_TRACE_COROUTINE("%p    size=%zu\n", CAST_VOID_P(coro), coro->stack.size);
    MINIMK_TRACE_COROUTINE("%p    top=0x%llx\n", CAST_VOID_P(coro), CAST_ULL(coro->stack.top));

    // Use assembly to synthesize the stack frame
    M_init_coro_stack(&coro->sp, coro->stack.top, reinterpret_cast<uintptr_t>(trampoline),
                      reinterpret_cast<uintptr_t>(sched));

    MINIMK_TRACE_COROUTINE("%p    sp=0x%llx\n", CAST_VOID_P(coro), CAST_ULL(coro->sp));

    // Mark as ready to run
    coro->state = CORO_RUNNABLE;
    MINIMK_TRACE_COROUTINE("%p NULL -> RUNNABLE\n", CAST_VOID_P(coro));
    return 0;
}

/// Testable implementation of minimk_runtime_coroutine_finish.
template <decltype(minimk_runtime_stack_free) M_stack_free = minimk_runtime_stack_free>
void minimk_runtime_coroutine_finish_impl(struct coroutine *coro) noexcept {
    // Delete the coroutine stack
    auto stack = &coro->stack;
    MINIMK_TRACE_COROUTINE("%p free_stack\n", CAST_VOID_P(coro));
    MINIMK_TRACE_COROUTINE("%p    base=0x%llx\n", CAST_VOID_P(coro), CAST_ULL(stack->base));
    MINIMK_TRACE_COROUTINE("%p    sp=0x%llx\n", CAST_VOID_P(coro), CAST_ULL(coro->sp));
    MINIMK_TRACE_COROUTINE("%p    size=%zu\n", CAST_VOID_P(coro), stack->size);
    (void)M_stack_free(stack);

    // Zero the structure and make it empty
    static_assert(CORO_NULL == 0, "expected CORO_NULL to be equal to zero");
    MINIMK_TRACE_COROUTINE("%p EXITED -> NULL\n", CAST_VOID_P(coro));
    *coro = {};
}

static inline void minimk_runtime_coroutine_maybe_resume_impl(struct coroutine *coro, uint64_t now,
                                                              short revents) noexcept {
    // Compute whether the coroutine was blocked on a timer and needs to be resumed
    if (coro->state == CORO_BLOCKED_ON_TIMER && now >= coro->deadline) {
        MINIMK_TRACE_COROUTINE("%p BLOCKED_ON_TIMER -> RUNNABLE\n", CAST_VOID_P(coro));
        coro->deadline = 0;
        coro->state = CORO_RUNNABLE;
        return;
    }

    // Compute whether the coroutine was blocked on I/O and needs to be resumed
    if (coro->state == CORO_BLOCKED_ON_IO && ((coro->events & revents) != 0 || now >= coro->deadline)) {
        MINIMK_TRACE_COROUTINE("%p BLOCKED_ON_IO -> RUNNABLE\n", CAST_VOID_P(coro));
        coro->deadline = 0;
        coro->state = CORO_RUNNABLE;
        coro->sock = minimk_syscall_invalid_socket;
        coro->events = 0;
        coro->revents = revents;
        return;
    }
}

static inline void minimk_runtime_coroutine_validate_stack_pointer_impl( //
        const char *context, struct coroutine *coro) noexcept {
    struct stack *stack = &coro->stack;
    uintptr_t sp = coro->sp;

    MINIMK_TRACE_COROUTINE("%p validate_stack=%s\n", CAST_VOID_P(coro), context);
    MINIMK_TRACE_COROUTINE("%p    bottom=0x%llx\n", CAST_VOID_P(coro), CAST_ULL(stack->bottom));
    MINIMK_TRACE_COROUTINE("%p    sp=0x%llx\n", CAST_VOID_P(coro), CAST_ULL(sp));
    MINIMK_TRACE_COROUTINE("%p    top=0x%llx\n", CAST_VOID_P(coro), CAST_ULL(stack->top));

    MINIMK_ASSERT(sp >= stack->bottom && sp < stack->top);
}

static inline void minimk_runtime_coroutine_suspend_timer_impl(struct coroutine *coro,
                                                               uint64_t deadline) noexcept {
    MINIMK_TRACE_COROUTINE("%p RUNNABLE -> BLOCKED_ON_TIMER\n", CAST_VOID_P(coro));
    coro->state = CORO_BLOCKED_ON_TIMER;
    coro->deadline = deadline;

    MINIMK_TRACE_COROUTINE("%p suspend_timer\n", CAST_VOID_P(coro));
    MINIMK_TRACE_COROUTINE("%p    deadline=%llu\n", CAST_VOID_P(coro), CAST_ULL(deadline));
}

static inline void minimk_runtime_coroutine_resume_timer_impl(struct coroutine *coro) noexcept {
    MINIMK_TRACE_COROUTINE("%p resume_timer\n", CAST_VOID_P(coro));
    coro->deadline = 0;
}

/// Parks the current coroutine until the given timeout expires.
template <decltype(minimk_time_monotonic_now) M_time_now = minimk_time_monotonic_now>
void minimk_runtime_coroutine_suspend_io_impl(struct coroutine *coro, minimk_syscall_socket_t sock,
                                              short events, uint64_t nanosec) noexcept {
    // Get the current monotonic clock reading
    uint64_t deadline = M_time_now();
    deadline = minimk_integer_u64_satadd(deadline, nanosec);

    // Prepare for suspending
    MINIMK_TRACE_COROUTINE("%p RUNNABLE -> BLOCKED_ON_IO\n", CAST_VOID_P(coro));
    coro->state = CORO_BLOCKED_ON_IO;
    coro->deadline = deadline;
    MINIMK_ASSERT(sock != minimk_syscall_invalid_socket);
    coro->sock = sock;
    coro->events = events;
    coro->revents = 0;

    MINIMK_TRACE_COROUTINE("%p suspend_io\n", CAST_VOID_P(coro));
    MINIMK_TRACE_COROUTINE("%p    fd=%llu\n", CAST_VOID_P(coro), CAST_ULL(sock));
    MINIMK_TRACE_COROUTINE("%p    events=%llu\n", CAST_VOID_P(coro), CAST_ULL(events));
    MINIMK_TRACE_COROUTINE("%p    deadline=%llu\n", CAST_VOID_P(coro), CAST_ULL(deadline));
}

static inline minimk_error_t minimk_runtime_coroutine_resume_io_impl(struct coroutine *coro,
                                                                     minimk_syscall_socket_t sock,
                                                                     short events) noexcept {
    // Finish cleaning up the coroutine state
    short revents = coro->revents;
    coro->revents = 0;
    MINIMK_ASSERT(coro->deadline == 0);
    MINIMK_ASSERT(coro->sock == minimk_syscall_invalid_socket);
    MINIMK_ASSERT(coro->events == 0);

    MINIMK_TRACE_COROUTINE("%p resume_io\n", CAST_VOID_P(coro));
    MINIMK_TRACE_COROUTINE("%p    fd=%llu\n", CAST_VOID_P(coro), CAST_ULL(sock));
    MINIMK_TRACE_COROUTINE("%p    events=%llu\n", CAST_VOID_P(coro), CAST_ULL(events));
    MINIMK_TRACE_COROUTINE("%p    revents=%llu\n", CAST_VOID_P(coro), CAST_ULL(revents));

    // We have a successful I/O suspend if the event we expected occurred.
    if ((revents & events) != 0) {
        return 0;
    }

    // We also have some kind of success if there is an error in the sense that
    // the caller should retry the I/O operation to get the error.
    if ((revents & minimk_syscall_pollerr) != 0) {
        return 0;
    }

    // Otherwise, it must have been an I/O timeout.
    return MINIMK_ETIMEDOUT;
}

static inline void minimk_runtime_coroutine_mark_as_exited_impl(struct coroutine *coro) noexcept {
    MINIMK_TRACE_COROUTINE("%p RUNNABLE -> EXITED\n", CAST_VOID_P(coro));
    coro->state = CORO_EXITED;
}

#endif // LIBMINIMK_RUNTIME_COROUTINE_HPP
