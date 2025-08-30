// File: libminimk/syscall/getsockopt_error_posix.hpp
// Purpose: getsockopt(2) on POSIX for SO_ERROR
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_GETSOCKOPT_ERROR_POSIX_HPP
#define LIBMINIMK_SYSCALL_GETSOCKOPT_ERROR_POSIX_HPP

#include "../errno/errno_posix.h" // for minimk_errno_map

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_geterrno
#include <minimk/trace.h>   // for MINIMK_TRACE_SYSCALL

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

    // Log that we're about to invoke the syscall
    MINIMK_TRACE_SYSCALL("getsockopt: fd=%d\n", sock);
    MINIMK_TRACE_SYSCALL("getsockopt: level=%s\n", "SOL_SOCKET");
    MINIMK_TRACE_SYSCALL("getsockopt: optname=%s\n", "SO_ERROR");

    // Clear the errno and issue the syscall
    M_minimk_syscall_clearerrno();
    int rv = M_sys_getsockopt(sock, SOL_SOCKET, SO_ERROR, static_cast<void *>(&soerr), &soerrlen);

    // Assign the result of the syscall
    minimk_error_t res = (rv == 0) ? 0 : M_minimk_syscall_geterrno();

    // Map the socket error to minimk_error_t
    *error = (rv == 0) ? M_minimk_errno_map(soerr) : 0;

    // Log the results of invoking the syscall
    MINIMK_TRACE_SYSCALL("getsockopt: result=%s\n", minimk_errno_name(res));
    MINIMK_TRACE_SYSCALL("getsockopt: soerr=%s\n", minimk_errno_name(*error));

    // Return the result
    return res;
}

#endif // LIBMINIMK_SYSCALL_GETSOCKOPT_ERROR_POSIX_HPP
