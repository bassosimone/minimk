// File: libminimk/syscall/setsockopt_nosigpipe_posix.hpp
// Purpose: setsockopt(2) on POSIX for SO_NOSIGPIPE
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SETSOCKOPT_NOSIGPIPE_POSIX_HPP
#define LIBMINIMK_SYSCALL_SETSOCKOPT_NOSIGPIPE_POSIX_HPP

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_geterrno

#include <sys/socket.h> // for setsockopt

#ifdef SO_NOSIGPIPE
#include <minimk/trace.h> // for MINIMK_TRACE_SYSCALL
#endif

/// Testable minimk_syscall_setsockopt_nosigpipe implementation.
template <decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
          decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
          decltype(setsockopt) M_sys_setsockopt = setsockopt>
minimk_error_t minimk_syscall_setsockopt_nosigpipe_impl(minimk_syscall_socket_t sock) noexcept {
#ifdef SO_NOSIGPIPE
    // Log that we're about to invoke the syscall
    MINIMK_TRACE_SYSCALL("setsockopt: fd=%d\n", sock);
    MINIMK_TRACE_SYSCALL("setsockopt: level=%s\n", "SOL_SOCKET");
    MINIMK_TRACE_SYSCALL("setsockopt: optname=%s\n", "SO_NOSIGPIPE");

    // Set SO_NOSIGPIPE to avoid SIGPIPE on closed connection writes
    int on = 1;
    M_minimk_syscall_clearerrno();
    int rv = M_sys_setsockopt((int)sock, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on));

    // Assign the result of the syscall
    minimk_error_t res = (rv == 0) ? 0 : M_minimk_syscall_geterrno();

    // Log the results of invoking the syscall
    MINIMK_TRACE_SYSCALL("setsockopt: result=%s\n", minimk_errno_name(res));

    // Return the result
    return res;
#else
    // SO_NOSIGPIPE is not available on this platform - this is a no-op
    (void)sock; // suppress unused parameter warning
    return 0;
#endif // SO_NOSIGPIPE
}

#endif // LIBMINIMK_SYSCALL_SETSOCKOPT_NOSIGPIPE_POSIX_HPP
