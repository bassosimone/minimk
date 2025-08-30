// File: libminimk/syscall/listen_posix.hpp
// Purpose: listen(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_LISTEN_POSIX_HPP
#define LIBMINIMK_SYSCALL_LISTEN_POSIX_HPP

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_geterrno

#include <sys/socket.h> // for listen

/// Testable minimk_syscall_listen implementation.
template <
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
        decltype(listen) M_sys_listen = listen>
minimk_error_t minimk_syscall_listen_impl(minimk_syscall_socket_t sock, int backlog) noexcept {
    M_minimk_syscall_clearerrno();
    int rv = M_sys_listen(sock, backlog);
    return (rv == 0) ? 0 : M_minimk_syscall_geterrno();
}

#endif // LIBMINIMK_SYSCALL_LISTEN_POSIX_HPP
