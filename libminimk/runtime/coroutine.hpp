// File: libminimk/runtime/coroutine.hpp
// Purpose: coroutine specific code
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_COROUTINE_HPP
#define LIBMINIMK_RUNTIME_COROUTINE_HPP

#include "../integer/u64.h" // for minimk_integer_u64_satadd

#include "coroutine.h" // for struct coroutine
#include "stack.h"     // for minimk_runtime_stack_alloc
#include "switch.h"    // for minimk_runtime_init_coro_stack

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_invalid_socket
#include <minimk/time.h>    // for minimk_time_monotonic_now
#include <minimk/trace.h>   // for MINIMK_TRACE

#include <stdint.h> // for int64_t

/// Testable implementation of minimk_runtime_coroutine_init.
template <
        decltype(minimk_runtime_stack_alloc) M_stack_alloc = minimk_runtime_stack_alloc,
        decltype(minimk_runtime_init_coro_stack) M_init_coro_stack = minimk_runtime_init_coro_stack>
minimk_error_t minimk_runtime_coroutine_init_impl(struct coroutine *coro, void (*trampoline)(void),
                                                  void (*entry)(void *opaque),
                                                  void *opaque) noexcept {
    // Zero initialize the whole coroutine
    *coro = {};

    // Initialize the entry
    coro->sock = minimk_syscall_invalid_socket;
    coro->entry = entry;
    coro->opaque = opaque;

    // Allocate a stack for the coroutine
    minimk_error_t rv = M_stack_alloc(&coro->stack);
    if (rv != 0) {
        return rv;
    }

    // Use assembly to synthesize the stack frame
    M_init_coro_stack(&coro->sp, coro->stack.top, reinterpret_cast<uintptr_t>(trampoline));

    MINIMK_TRACE("trace: coroutine<0x%llx> stack layout:\n",
                 reinterpret_cast<unsigned long long>(coro));
    MINIMK_TRACE("    stack_top: 0x%llx\n", static_cast<unsigned long long>(coro->stack.top));
    MINIMK_TRACE("    sp: 0x%llx\n", static_cast<unsigned long long>(coro->sp));

    // Mark as ready to run
    coro->state = CORO_RUNNABLE;
    MINIMK_TRACE("trace: coroutine<0x%llx> RUNNABLE\n", reinterpret_cast<unsigned long long>(coro));
    return 0;
}

/// Testable implementation of minimk_runtime_coroutine_finish.
template <decltype(minimk_runtime_stack_free) M_stack_free = minimk_runtime_stack_free>
void minimk_runtime_coroutine_finish_impl(struct coroutine *coro) noexcept {
    (void)M_stack_free(&coro->stack);
    MINIMK_TRACE("trace: coroutine<0x%llx> NULL\n", reinterpret_cast<unsigned long long>(coro));
    *coro = {}; // The zero state implies empty slot
}

static inline void minimk_runtime_coroutine_maybe_resume_impl(struct coroutine *coro, uint64_t now,
                                                              short revents) noexcept {
    // Compute whether the coroutine was blocked on a timer and needs to be resumed
    if (coro->state == CORO_BLOCKED_ON_TIMER && now >= coro->deadline) {
        coro->deadline = 0;
        coro->state = CORO_RUNNABLE;
        return;
    }

    // Compute whether the coroutine was blocked on I/O and needs to be resumed
    if (coro->state == CORO_BLOCKED_ON_IO &&
        ((coro->events & revents) != 0 || now >= coro->deadline)) {
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

    MINIMK_TRACE("trace: %s: validating sp=0x%llx in stack[0x%llx, 0x%llx)\n", context,
                 static_cast<unsigned long long>(sp),
                 static_cast<unsigned long long>(stack->bottom),
                 static_cast<unsigned long long>(stack->top));

    MINIMK_ASSERT(sp >= stack->bottom && sp < stack->top);
}

/// Parks the current goroutine until the given timeout expires.
template <decltype(minimk_time_monotonic_now) M_time_now = minimk_time_monotonic_now>
void minimk_runtime_coroutine_suspend_io_impl(struct coroutine *coro, minimk_syscall_socket_t sock,
                                              short events, uint64_t nanosec) noexcept {
    // Get the current monotonic clock reading
    uint64_t deadline = M_time_now();
    deadline = minimk_integer_u64_satadd(deadline, nanosec);

    // Prepare for suspending
    coro->state = CORO_BLOCKED_ON_IO;
    coro->deadline = deadline;
    MINIMK_ASSERT(sock != minimk_syscall_invalid_socket);
    coro->sock = sock;
    coro->events = events;
    coro->revents = 0;

    MINIMK_TRACE("trace: suspend coroutine<0x%llx> on fd=%llu events=%llu\n",
                 reinterpret_cast<unsigned long long>(coro), static_cast<unsigned long long>(sock),
                 static_cast<unsigned long long>(events));
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

    MINIMK_TRACE("trace: resume coroutine<0x%llx> on fd=%llu events=%llu revents=%llu\n",
                 reinterpret_cast<unsigned long long>(coro), static_cast<unsigned long long>(sock),
                 static_cast<unsigned long long>(events), static_cast<unsigned long long>(revents));

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

#endif // LIBMINIMK_RUNTIME_COROUTINE_HPP
