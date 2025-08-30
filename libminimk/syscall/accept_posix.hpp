// File: libminimk/syscall/accept_posix.hpp
// Purpose: accept(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_ACCEPT_POSIX_HPP
#define LIBMINIMK_SYSCALL_ACCEPT_POSIX_HPP

#include "errno.h"  // for minimk_syscall_clearerrno
#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/errno.h> // for minimk_error_t

#include <sys/socket.h> // for accept

/// Testable minimk_syscall_accept implementation.
template <
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
        decltype(accept) M_sys_accept = accept>
minimk_error_t minimk_syscall_accept_impl(minimk_syscall_socket_t *client_sock,
                                          minimk_syscall_socket_t sock) noexcept {
    M_minimk_syscall_clearerrno();
    *client_sock = -1;
    int client_fd = M_sys_accept((int)sock, nullptr, nullptr);
    if (client_fd == -1) {
        return M_minimk_syscall_geterrno();
    }
    *client_sock = (minimk_syscall_socket_t)client_fd;
    return 0;
}

#endif // LIBMINIMK_SYSCALL_ACCEPT_POSIX_HPP
