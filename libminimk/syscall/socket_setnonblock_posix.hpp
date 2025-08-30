// File: libminimk/syscall/socket_setnonblock_posix.hpp
// Purpose: fcntl(2) on POSIX for setting socket nonblocking
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SOCKET_SETNONBLOCK_POSIX_HPP
#define LIBMINIMK_SYSCALL_SOCKET_SETNONBLOCK_POSIX_HPP

#include "errno.h"  // for minimk_syscall_geterrno
#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/errno.h> // for minimk_error_t

#include <fcntl.h> // for fcntl

/// Testable minimk_syscall_socket_setnonblock implementation.
template <
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
        decltype(fcntl) M_sys_fcntl2 = fcntl, decltype(fcntl) M_sys_fcntl3 = fcntl>
minimk_error_t minimk_syscall_socket_setnonblock_impl(minimk_syscall_socket_t sock) noexcept {
    // Get the flags
    M_minimk_syscall_clearerrno();
    int flags = M_sys_fcntl2((int)sock, F_GETFL);
    if (flags == -1) {
        return M_minimk_syscall_geterrno();
    }

    // Enable nonblocking
    flags |= O_NONBLOCK;

    // Set the flags
    M_minimk_syscall_clearerrno();
    if (M_sys_fcntl3((int)sock, F_SETFL, flags) == -1) {
        return M_minimk_syscall_geterrno();
    }
    return 0;
}

#endif // LIBMINIMK_SYSCALL_SOCKET_SETNONBLOCK_POSIX_HPP
