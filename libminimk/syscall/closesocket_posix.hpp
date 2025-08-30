// File: libminimk/syscall/closesocket_posix.hpp
// Purpose: close(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_CLOSESOCKET_POSIX_HPP
#define LIBMINIMK_SYSCALL_CLOSESOCKET_POSIX_HPP

#include "../errno/errno.h" // for minimk_errno_get

#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/errno.h> // for minimk_errno_clear

#include <unistd.h> // for close

/// Testable minimk_syscall_closesocket implementation.
template <decltype(minimk_errno_clear) minimk_errno_clear__ = minimk_errno_clear,
          decltype(minimk_errno_get) minimk_errno_get__ = minimk_errno_get,
          decltype(close) sys_close__ = close>
minimk_error_t minimk_syscall_closesocket__(minimk_syscall_socket_t *sock) noexcept {
    // Clear the errno ahead of the syscall
    minimk_errno_clear__();

    // Issue the syscall proper
    int rv = sys_close__((int)*sock);

    // Set socket to invalid state
    *sock = -1;

    // Return success or error
    return (rv == 0) ? 0 : minimk_errno_get__();
}

#endif // LIBMINIMK_SYSCALL_CLOSESOCKET_POSIX_HPP
