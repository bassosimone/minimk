// File: libminimk/syscall/socket_setnonblock_posix.hpp
// Purpose: fcntl(2) on POSIX for setting socket nonblocking
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SOCKET_SETNONBLOCK_POSIX_HPP
#define LIBMINIMK_SYSCALL_SOCKET_SETNONBLOCK_POSIX_HPP

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_geterrno
#include <minimk/trace.h>   // for MINIMK_TRACE_SYSCALL

#include <fcntl.h> // for fcntl

/// Testable minimk_syscall_socket_setnonblock implementation.
template <decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
          decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
          decltype(fcntl) M_sys_fcntl2 = fcntl, decltype(fcntl) M_sys_fcntl3 = fcntl>
minimk_error_t minimk_syscall_socket_setnonblock_impl(minimk_syscall_socket_t sock) noexcept {
    // Log that we're about to get the flags
    MINIMK_TRACE_SYSCALL("fcntl: fd=%d\n", sock);
    MINIMK_TRACE_SYSCALL("fcntl: cmd=%s\n", "F_GETFL");

    // Get the flags
    M_minimk_syscall_clearerrno();
    int flags = M_sys_fcntl2(sock, F_GETFL);
    if (flags == -1) {
        minimk_error_t res = M_minimk_syscall_geterrno();
        MINIMK_TRACE_SYSCALL("fcntl: result=%s\n", minimk_errno_name(res));
        return res;
    }

    // Log the result of getting flags
    MINIMK_TRACE_SYSCALL("fcntl: result=%s\n", minimk_errno_name(0));
    MINIMK_TRACE_SYSCALL("fcntl: flags=%d\n", flags);

    // Enable nonblocking
    flags |= O_NONBLOCK;

    // Log that we're about to set the flags
    MINIMK_TRACE_SYSCALL("fcntl: fd=%d\n", sock);
    MINIMK_TRACE_SYSCALL("fcntl: cmd=%s\n", "F_SETFL");
    MINIMK_TRACE_SYSCALL("fcntl: flags=%d\n", flags);

    // Set the flags
    M_minimk_syscall_clearerrno();
    int rv = M_sys_fcntl3(sock, F_SETFL, flags);
    minimk_error_t res = (rv == -1) ? M_minimk_syscall_geterrno() : 0;

    // Log the result of setting flags
    MINIMK_TRACE_SYSCALL("fcntl: result=%s\n", minimk_errno_name(res));

    // Return the result
    return res;
}

#endif // LIBMINIMK_SYSCALL_SOCKET_SETNONBLOCK_POSIX_HPP
