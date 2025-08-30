// File: libminimk/syscall/closesocket_posix.hpp
// Purpose: close(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_CLOSESOCKET_POSIX_HPP
#define LIBMINIMK_SYSCALL_CLOSESOCKET_POSIX_HPP

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_geterrno

#include <unistd.h> // for close

/// Testable minimk_syscall_closesocket implementation.
template <
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
        decltype(close) M_sys_close = close>
minimk_error_t minimk_syscall_closesocket_impl(minimk_syscall_socket_t *sock) noexcept {
    // Clear the errno ahead of the syscall
    M_minimk_syscall_clearerrno();

    // Issue the syscall proper
    int rv = M_sys_close(*sock);

    // Set socket to invalid state
    *sock = -1;

    // Return success or error
    return (rv == 0) ? 0 : M_minimk_syscall_geterrno();
}

#endif // LIBMINIMK_SYSCALL_CLOSESOCKET_POSIX_HPP
