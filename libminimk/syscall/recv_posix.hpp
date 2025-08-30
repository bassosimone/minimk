// File: libminimk/syscall/recv_posix.hpp
// Purpose: recv(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_RECV_POSIX_HPP
#define LIBMINIMK_SYSCALL_RECV_POSIX_HPP

#include "errno.h"  // for minimk_syscall_geterrno
#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/errno.h> // for minimk_error_t
#include <minimk/trace.h> // for MINIMK_TRACE

#include <sys/socket.h> // for recv
#include <sys/types.h>  // for ssize_t

#include <limits.h> // for SSIZE_MAX
#include <stddef.h> // for size_t

/// Testable minimk_syscall_recv implementation.
template <
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
        decltype(recv) M_sys_recv = recv>
minimk_error_t minimk_syscall_recv_impl(minimk_syscall_socket_t sock, void *data, size_t count,
                                        size_t *nread) noexcept {
    // Initialize output parameter immediately
    *nread = 0;

    // As documented, reject zero-byte reads
    if (count <= 0) {
        MINIMK_TRACE("trace: suspicious recv 0x%llx with zero bytes size\n",
                     static_cast<unsigned long long>(sock));
        return MINIMK_EINVAL;
    }

    // Prepare flags - start with none, add MSG_NOSIGNAL if available
    int flags = 0;
#ifdef MSG_NOSIGNAL
    flags |= MSG_NOSIGNAL;
#endif

    // Issue the recv system call proper
    M_minimk_syscall_clearerrno();
    count = (count <= SSIZE_MAX) ? count : SSIZE_MAX;
    ssize_t rv = M_sys_recv(sock, data, count, flags);

    // Handle the case of error
    if (rv == -1) {
        return M_minimk_syscall_geterrno();
    }

    // Handle the case of EOF
    if (rv == 0) {
        return MINIMK_EOF;
    }

    // Handle the case of success
    *nread = static_cast<size_t>(rv);
    return 0;
}

#endif // LIBMINIMK_SYSCALL_RECV_POSIX_HPP
