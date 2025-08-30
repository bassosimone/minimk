// File: libminimk/syscall/send_posix.hpp
// Purpose: send(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SEND_POSIX_HPP
#define LIBMINIMK_SYSCALL_SEND_POSIX_HPP

#include "errno.h"             // for minimk_syscall_geterrno
#include "../runtime/trace.h" // for MINIMK_TRACE
#include "socket.h"           // for minimk_syscall_socket_t

#include <minimk/errno.h> // for minimk_error_t

#include <sys/socket.h> // for send
#include <sys/types.h>  // for ssize_t

#include <limits.h> // for SSIZE_MAX
#include <stddef.h> // for size_t

/// Testable minimk_syscall_send implementation.
template <decltype(minimk_syscall_clearerrno) minimk_syscall_clearerrno__ = minimk_syscall_clearerrno,
          decltype(minimk_syscall_geterrno) minimk_syscall_geterrno__ = minimk_syscall_geterrno,
          decltype(send) sys_send__ = send>
minimk_error_t minimk_syscall_send__(minimk_syscall_socket_t sock, const void *data, size_t count,
                                     size_t *nwritten) noexcept {
    // Initialize output parameter immediately
    *nwritten = 0;

    // As documented, reject zero-byte writes
    if (count <= 0) {
        MINIMK_TRACE("trace: suspicious send 0x%llx with zero bytes size\n",
                     (unsigned long long)sock);
        return MINIMK_EINVAL;
    }

    // Prepare flags - start with none, add MSG_NOSIGNAL if available
    int flags = 0;
#ifdef MSG_NOSIGNAL
    flags |= MSG_NOSIGNAL;
#endif

    // Issue the send system call proper
    minimk_syscall_clearerrno__();
    count = (count <= SSIZE_MAX) ? count : SSIZE_MAX;
    ssize_t rv = sys_send__((int)sock, data, count, flags);

    // Handle the case of error
    if (rv == -1) {
        return minimk_syscall_geterrno__();
    }

    // Handle the case of success
    *nwritten = (size_t)rv;
    return 0;
}

#endif // LIBMINIMK_SYSCALL_SEND_POSIX_HPP
