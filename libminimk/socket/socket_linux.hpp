// File: libminimk/socket/socket_linux.hpp
// Purpose: socket library implemented for linux
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SOCKET_SOCKET_LINUX_HPP
#define LIBMINIMK_SOCKET_SOCKET_LINUX_HPP

#include "../errno/errno.h"   // for minimk_errno_get
#include "../runtime/trace.h" // for MINIMK_TRACE

#include "socket.h" // for minimk_socket_t

#include <minimk/errno.h> // for minimk_errno_clear

#include <sys/socket.h> // for recv
#include <sys/types.h>  // for ssize_t

#include <fcntl.h>  // for fcntl
#include <limits.h> // for SSIZE_MAX
#include <netdb.h>  // for getaddrinfo
#include <stddef.h> // for size_t
#include <unistd.h> // for close

// Testable minimk_socket_setnonblock implementation.
template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(minimk_errno_get) __minimk_errno_get = minimk_errno_get,
          decltype(fcntl) __sys_fcntl2 = fcntl, decltype(fcntl) __sys_fcntl3 = fcntl>
minimk_error_t __minimk_socket_setnonblock(minimk_socket_t sock) noexcept {
    // Get the flags
    __minimk_errno_clear();
    int flags = __sys_fcntl2((int)sock, F_GETFL);
    if (flags == -1) {
        return __minimk_errno_get();
    }

    // Enable nonblocking
    flags |= O_NONBLOCK;

    // Set the flags
    __minimk_errno_clear();
    if (__sys_fcntl3((int)sock, F_SETFL, flags) == -1) {
        return __minimk_errno_get();
    }
    return 0;
}

// Testable minimk_socket_recv implementation.
template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(minimk_errno_get) __minimk_errno_get = minimk_errno_get,
          decltype(recv) __sys_recv = recv>
minimk_error_t __minimk_socket_recv(minimk_socket_t sock, void *data, size_t count,
                                    size_t *nread) noexcept {
    // Initialize output parameter immediately
    *nread = 0;

    // As documented, reject zero-byte reads
    if (count <= 0) {
        MINIMK_TRACE("trace: suspicious recv 0x%llx with zero bytes size\n",
                     (unsigned long long)sock);
        return MINIMK_EINVAL;
    }

    // Issue the recv system call proper
    __minimk_errno_clear();
    count = (count <= SSIZE_MAX) ? count : SSIZE_MAX;
    ssize_t rv = __sys_recv((int)sock, data, count, MSG_NOSIGNAL);

    // Handle the case of error
    if (rv == -1) {
        return __minimk_errno_get();
    }

    // Handle the case of EOF
    if (rv == 0) {
        return MINIMK_EOF;
    }

    // Handle the case of success
    *nread = (size_t)rv;
    return 0;
}

// Testable minimk_socket_send implementation.
template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(minimk_errno_get) __minimk_errno_get = minimk_errno_get,
          decltype(send) __sys_send = send>
minimk_error_t __minimk_socket_send(minimk_socket_t sock, const void *data, size_t count,
                                    size_t *nwritten) noexcept {
    // Initialize output parameter immediately
    *nwritten = 0;

    // As documented, reject zero-byte reads
    if (count <= 0) {
        MINIMK_TRACE("trace: suspicious send 0x%llx with zero bytes size\n",
                     (unsigned long long)sock);
        return MINIMK_EINVAL;
    }

    // Issue the send system call proper
    __minimk_errno_clear();
    count = (count <= SSIZE_MAX) ? count : SSIZE_MAX;
    ssize_t rv = __sys_send((int)sock, data, count, MSG_NOSIGNAL);

    // Handle the case of error
    if (rv == -1) {
        return __minimk_errno_get();
    }

    // Handle the case of success
    *nwritten = (size_t)rv;
    return 0;
}

#endif // LIBMINIMK_SOCKET_SOCKET_LINUX_HPP
