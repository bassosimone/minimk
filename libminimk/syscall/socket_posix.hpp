// File: libminimk/syscall/socket_posix.hpp
// Purpose: socket(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SOCKET_POSIX_HPP
#define LIBMINIMK_SYSCALL_SOCKET_POSIX_HPP

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_clearerrno

#include <sys/socket.h> // for socket

/// Testable minimk_syscall_socket implementation.
template <
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
        decltype(socket) M_sys_socket = socket>
minimk_error_t minimk_syscall_socket_impl(minimk_syscall_socket_t *sock, int domain, int type,
                                          int protocol) noexcept {
    // Clear the errno ahead of the syscall
    M_minimk_syscall_clearerrno();

    // Issue the syscall proper
    int fdesc = M_sys_socket(domain, type, protocol);

    // Determine whether it succeeded
    int ok = (fdesc >= 0);

    // Assign the resulting socket depending on whether it succeeded
    *sock = ok ? fdesc : -1;

    // Assign retval depending on whether it succeeded
    return ok ? 0 : M_minimk_syscall_geterrno();
}

#endif // LIBMINIMK_SYSCALL_SOCKET_POSIX_HPP
