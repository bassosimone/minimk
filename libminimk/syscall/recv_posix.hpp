// File: libminimk/syscall/recv_posix.hpp
// Purpose: recv(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_RECV_POSIX_HPP
#define LIBMINIMK_SYSCALL_RECV_POSIX_HPP

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_geterrno
#include <minimk/time.h>    // for minimk_time_monotonic_now
#include <minimk/trace.h>   // for MINIMK_TRACE_SYSCALL

#include <sys/socket.h> // for recv
#include <sys/types.h>  // for ssize_t

#include <limits.h> // for SSIZE_MAX
#include <stddef.h> // for size_t

/// Testable minimk_syscall_recv implementation.
template <decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
          decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
          decltype(recv) M_sys_recv = recv>
minimk_error_t minimk_syscall_recv_impl(minimk_syscall_socket_t sock, void *data, size_t count,
                                        size_t *nread) noexcept {
    // Initialize output parameter immediately
    *nread = 0;

    // As documented, reject zero-byte reads
    if (count <= 0) {
        MINIMK_TRACE_SYSCALL("recv: suspicious fd=%d with zero bytes count=%zu\n", sock, count);
        return MINIMK_EINVAL;
    }

    // Prepare flags - start with none, add MSG_NOSIGNAL if available
    int flags = 0;
#ifdef MSG_NOSIGNAL
    flags |= MSG_NOSIGNAL;
#endif

    // Log that we're about to invoke the syscall
    count = (count <= SSIZE_MAX) ? count : SSIZE_MAX;
    MINIMK_TRACE_SYSCALL("recv: fd=%d\n", sock);
    MINIMK_TRACE_SYSCALL("recv: count=%zu\n", count);
    MINIMK_TRACE_SYSCALL("recv: t0=%lu\n", minimk_time_monotonic_now());

    // Clear the errno and issue the syscall
    M_minimk_syscall_clearerrno();
    ssize_t rv = M_sys_recv(sock, data, count, flags);

    // Assign the result branchlessly
    *nread = (rv > 0) ? static_cast<size_t>(rv) : 0;
    minimk_error_t res = (rv == -1) ? M_minimk_syscall_geterrno() : (rv == 0) ? MINIMK_EOF : 0;

    // Log the results of invoking the syscall
    MINIMK_TRACE_SYSCALL("recv: result=%s\n", minimk_errno_name(res));
    MINIMK_TRACE_SYSCALL("recv: nread=%zu\n", *nread);
    MINIMK_TRACE_SYSCALL("recv: t1=%lu\n", minimk_time_monotonic_now());

    // Return the result
    return res;
}

#endif // LIBMINIMK_SYSCALL_RECV_POSIX_HPP
