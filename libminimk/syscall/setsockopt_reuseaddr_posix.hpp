// File: libminimk/syscall/setsockopt_reuseaddr_posix.hpp
// Purpose: setsockopt(2) on POSIX for SO_REUSEADDR
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SETSOCKOPT_REUSEADDR_POSIX_HPP
#define LIBMINIMK_SYSCALL_SETSOCKOPT_REUSEADDR_POSIX_HPP

#include <minimk/cdefs.h>   // for MINIMK_ALWAYS_INLINE
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_geterrno
#include <minimk/trace.h>   // for MINIMK_TRACE_SYSCALL

#include <sys/socket.h> // for setsockopt

/// Testable minimk_syscall_setsockopt_reuseaddr implementation.
template <decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
          decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
          decltype(setsockopt) M_sys_setsockopt = setsockopt>
MINIMK_ALWAYS_INLINE minimk_error_t
minimk_syscall_setsockopt_reuseaddr_impl(minimk_syscall_socket_t sock) noexcept {
    // Log that we're about to invoke the syscall
    MINIMK_TRACE_SYSCALL("setsockopt: fd=%d\n", sock);
    MINIMK_TRACE_SYSCALL("setsockopt: level=%s\n", "SOL_SOCKET");
    MINIMK_TRACE_SYSCALL("setsockopt: optname=%s\n", "SO_REUSEADDR");

    // Set SO_REUSEADDR to allow reuse of local addresses
    int on = 1;
    M_minimk_syscall_clearerrno();
    int rv = M_sys_setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // Assign the result of the syscall
    minimk_error_t res = (rv == 0) ? 0 : M_minimk_syscall_geterrno();

    // Log the results of invoking the syscall
    MINIMK_TRACE_SYSCALL("setsockopt: result=%s\n", minimk_errno_name(res));

    // Return the result
    return res;
}

#endif // LIBMINIMK_SYSCALL_SETSOCKOPT_REUSEADDR_POSIX_HPP
