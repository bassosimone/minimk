// File: libminimk/syscall/getsockopt_error_posix.hpp
// Purpose: getsockopt(2) on POSIX for SO_ERROR
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_GETSOCKOPT_ERROR_POSIX_HPP
#define LIBMINIMK_SYSCALL_GETSOCKOPT_ERROR_POSIX_HPP

#include "errno.h"  // for minimk_syscall_geterrno
#include "socket.h" // for minimk_syscall_socket_t

#include "../errno/errno_posix.h" // for minimk_errno_map

#include <minimk/errno.h> // for minimk_error_t

#include <sys/socket.h> // for getsockopt

/// Testable minimk_syscall_getsockopt_error implementation.
template <
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
        decltype(getsockopt) M_sys_getsockopt = getsockopt,
        decltype(minimk_errno_map) M_minimk_errno_map = minimk_errno_map>
minimk_error_t minimk_syscall_getsockopt_error_impl(minimk_syscall_socket_t sock,
                                                    minimk_error_t *error) noexcept {
    int soerr = 0;
    socklen_t soerrlen = sizeof(soerr);

    // Clear errno before the syscall
    M_minimk_syscall_clearerrno();

    // Get the socket error
    if (M_sys_getsockopt(sock, SOL_SOCKET, SO_ERROR, static_cast<void *>(&soerr), &soerrlen) != 0) {
        return M_minimk_syscall_geterrno();
    }

    // Map the socket error to minimk_error_t
    *error = M_minimk_errno_map(soerr);
    return 0;
}

#endif // LIBMINIMK_SYSCALL_GETSOCKOPT_ERROR_POSIX_HPP
