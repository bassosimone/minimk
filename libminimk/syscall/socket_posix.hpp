// File: libminimk/syscall/socket_posix.hpp
// Purpose: socket(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SOCKET_POSIX_HPP
#define LIBMINIMK_SYSCALL_SOCKET_POSIX_HPP

#include "errno.h"  // for minimk_syscall_clearerrno
#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/errno.h> // for minimk_error_t

#include <sys/socket.h> // for socket

/// Testable minimk_syscall_socket implementation.
template <decltype(minimk_syscall_clearerrno) minimk_syscall_clearerrno__ = minimk_syscall_clearerrno,
          decltype(minimk_syscall_geterrno) minimk_syscall_geterrno__ = minimk_syscall_geterrno,
          decltype(socket) sys_socket__ = socket>
minimk_error_t minimk_syscall_socket__(minimk_syscall_socket_t *sock, int domain, int type,
                                       int protocol) noexcept {
    // Clear the errno ahead of the syscall
    minimk_syscall_clearerrno__();

    // Issue the syscall proper
    int fdesc = sys_socket__(domain, type, protocol);

    // Determine whether it succeeded
    int ok = (fdesc >= 0);

    // Assign the resulting socket depending on whether it succeeded
    *sock = ok ? (minimk_syscall_socket_t)fdesc : -1;

    // Assign retval depending on whether it succeeded
    return ok ? 0 : minimk_syscall_geterrno__();
}

#endif // LIBMINIMK_SYSCALL_SOCKET_POSIX_HPP
