// File: libminimk/runtime/coroutine.cpp
// Purpose: coroutine specific code
// SPDX-License-Identifier: GPL-3.0-or-later

#include "coroutine.hpp" // for minimk_runtime_coroutine_init_impl
#include "coroutine.h"   // for minimk_runtime_coroutine_*

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_invalid_socket

minimk_error_t minimk_runtime_coroutine_init(struct coroutine *coro, void (*trampoline)(void),
                                             void (*entry)(void *opaque), void *opaque) noexcept {
    return minimk_runtime_coroutine_init_impl(coro, trampoline, entry, opaque);
}

void minimk_runtime_coroutine_finish(struct coroutine *coro) noexcept {
    minimk_runtime_coroutine_finish_impl(coro);
}

void minimk_runtime_coroutine_maybe_resume(struct coroutine *coro, uint64_t now,
                                           short revents) noexcept {
    return minimk_runtime_coroutine_maybe_resume_impl(coro, now, revents);
}

void minimk_runtime_coroutine_validate_stack_pointer( //
        const char *context, struct coroutine *coro) noexcept {
    minimk_runtime_coroutine_validate_stack_pointer_impl(context, coro);
}

void minimk_runtime_coroutine_suspend_io(struct coroutine *coro, minimk_syscall_socket_t sock,
                                         short events, uint64_t nanosec) noexcept {
    minimk_runtime_coroutine_suspend_io_impl(coro, sock, events, nanosec);
}

minimk_error_t minimk_runtime_coroutine_resume_io(struct coroutine *coro,
                                                  minimk_syscall_socket_t sock,
                                                  short events) noexcept {
    return minimk_runtime_coroutine_resume_io_impl(coro, sock, events);
}
