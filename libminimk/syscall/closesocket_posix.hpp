// File: libminimk/syscall/closesocket_posix.hpp
// Purpose: close(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_CLOSESOCKET_POSIX_HPP
#define LIBMINIMK_SYSCALL_CLOSESOCKET_POSIX_HPP

#include "errno.h"  // for minimk_syscall_geterrno
#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/errno.h> // for minimk_error_t

#include <unistd.h> // for close

/// Testable minimk_syscall_closesocket implementation.
template <decltype(minimk_syscall_clearerrno) minimk_syscall_clearerrno__ = minimk_syscall_clearerrno,
          decltype(minimk_syscall_geterrno) minimk_syscall_geterrno__ = minimk_syscall_geterrno,
          decltype(close) sys_close__ = close>
minimk_error_t minimk_syscall_closesocket__(minimk_syscall_socket_t *sock) noexcept {
    // Clear the errno ahead of the syscall
    minimk_syscall_clearerrno__();

    // Issue the syscall proper
    int rv = sys_close__((int)*sock);

    // Set socket to invalid state
    *sock = -1;

    // Return success or error
    return (rv == 0) ? 0 : minimk_syscall_geterrno__();
}

#endif // LIBMINIMK_SYSCALL_CLOSESOCKET_POSIX_HPP
