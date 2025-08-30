// File: libminimk/syscall/socket_posix.hpp
// Purpose: socket(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SOCKET_POSIX_HPP
#define LIBMINIMK_SYSCALL_SOCKET_POSIX_HPP

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_clearerrno
#include <minimk/trace.h>   // for MINIMK_TRACE_SYSCALL

#include <sys/socket.h> // for socket

/// Testable minimk_syscall_socket implementation.
template <
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
        decltype(socket) M_sys_socket = socket>
minimk_error_t minimk_syscall_socket_impl(minimk_syscall_socket_t *sock, int domain, int type,
                                          int protocol) noexcept {
    // Log that we're about to invoke the syscall
    MINIMK_TRACE_SYSCALL("socket: domain=%d\n", domain);
    MINIMK_TRACE_SYSCALL("socket: type=%d\n", type);
    MINIMK_TRACE_SYSCALL("socket: protocol=%d\n", protocol);

    // Clear the errno and issue the syscall
    M_minimk_syscall_clearerrno();
    int fdesc = M_sys_socket(domain, type, protocol);

    // Assign the resulting socket and result branchlessly
    *sock = (fdesc >= 0) ? fdesc : -1;
    minimk_error_t res = (fdesc >= 0) ? 0 : M_minimk_syscall_geterrno();

    // Log the results of invoking the syscall
    MINIMK_TRACE_SYSCALL("socket: result=%s\n", minimk_errno_name(res));
    MINIMK_TRACE_SYSCALL("socket: fd=%d\n", *sock);

    // Return the result
    return res;
}

#endif // LIBMINIMK_SYSCALL_SOCKET_POSIX_HPP
