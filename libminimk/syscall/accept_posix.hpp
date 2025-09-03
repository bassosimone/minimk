// File: libminimk/syscall/accept_posix.hpp
// Purpose: accept(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_ACCEPT_POSIX_HPP
#define LIBMINIMK_SYSCALL_ACCEPT_POSIX_HPP

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_clearerrno
#include <minimk/time.h>    // for minimk_time_monotonic_now
#include <minimk/trace.h>   // for MINIMK_TRACE_SYSCALL

#include <sys/socket.h> // for accept

/// Testable minimk_syscall_accept implementation.
template <decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
          decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
          decltype(accept) M_sys_accept = accept>
minimk_error_t minimk_syscall_accept_impl(minimk_syscall_socket_t *client_sock,
                                          minimk_syscall_socket_t sock) noexcept {
    // Log that we're about to invoke the syscall
    MINIMK_TRACE_SYSCALL("accept: t0=%lu\n", minimk_time_monotonic_now());
    MINIMK_TRACE_SYSCALL("accept: lfd=%d\n", sock);

    // Clear the errno and issue the syscall
    M_minimk_syscall_clearerrno();
    *client_sock = M_sys_accept(sock, nullptr, nullptr);

    // Assign the result of the syscall
    minimk_error_t res = (*client_sock == -1) ? M_minimk_syscall_geterrno() : 0;

    // Log the results of invoking the syscall
    MINIMK_TRACE_SYSCALL("accept: t1=%lu\n", minimk_time_monotonic_now());
    MINIMK_TRACE_SYSCALL("accept: result=%s\n", minimk_errno_name(res));
    MINIMK_TRACE_SYSCALL("accept: cfd=%d\n", *client_sock);

    // Return the result
    return res;
}

#endif // LIBMINIMK_SYSCALL_ACCEPT_POSIX_HPP
