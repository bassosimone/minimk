// File: libminimk/syscall/socket_posix.hpp
// Purpose: socket(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SOCKET_POSIX_HPP
#define LIBMINIMK_SYSCALL_SOCKET_POSIX_HPP

#include "../errno/errno.h" // for minimk_errno_get

#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/errno.h> // for minimk_errno_clear

#include <sys/socket.h> // for socket

/// Testable minimk_syscall_socket implementation.
template <decltype(minimk_errno_clear) minimk_errno_clear__ = minimk_errno_clear,
          decltype(minimk_errno_get) minimk_errno_get__ = minimk_errno_get,
          decltype(socket) sys_socket__ = socket>
minimk_error_t minimk_syscall_socket__(minimk_syscall_socket_t *sock, int domain, int type,
                                       int protocol) noexcept {
    // Clear the errno ahead of the syscall
    minimk_errno_clear__();

    // Issue the syscall proper
    int fdesc = sys_socket__(domain, type, protocol);

    // Determine whether it succeeded
    int ok = (fdesc >= 0);

    // Assign the resulting socket depending on whether it succeeded
    *sock = ok ? (minimk_syscall_socket_t)fdesc : -1;

    // Assign retval depending on whether it succeeded
    return ok ? 0 : minimk_errno_get__();
}

#endif // LIBMINIMK_SYSCALL_SOCKET_POSIX_HPP
