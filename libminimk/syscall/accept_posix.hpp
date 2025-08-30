// File: libminimk/syscall/accept_posix.hpp
// Purpose: accept(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_ACCEPT_POSIX_HPP
#define LIBMINIMK_SYSCALL_ACCEPT_POSIX_HPP

#include "../errno/errno.h" // for minimk_errno_get

#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/errno.h> // for minimk_errno_clear

#include <sys/socket.h> // for accept

/// Testable minimk_syscall_accept implementation.
template <decltype(minimk_errno_clear) minimk_errno_clear__ = minimk_errno_clear,
          decltype(minimk_errno_get) minimk_errno_get__ = minimk_errno_get,
          decltype(accept) sys_accept__ = accept>
minimk_error_t minimk_syscall_accept__(minimk_syscall_socket_t *client_sock,
                                       minimk_syscall_socket_t sock) noexcept {
    minimk_errno_clear__();
    *client_sock = -1;
    int client_fd = sys_accept__((int)sock, nullptr, nullptr);
    if (client_fd == -1) {
        return minimk_errno_get__();
    }
    *client_sock = (minimk_syscall_socket_t)client_fd;
    return 0;
}

#endif // LIBMINIMK_SYSCALL_ACCEPT_POSIX_HPP
