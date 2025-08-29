// File: libminimk/socket/socket_linux.hpp
// Purpose: socket library implemented for linux
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SOCKET_SOCKET_LINUX_HPP
#define LIBMINIMK_SOCKET_SOCKET_LINUX_HPP

#include <minimk/errno.h>  // for minimk_errno_clear
#include <minimk/socket.h> // for minimk_socket_t

#include <sys/socket.h> // for recv
#include <sys/types.h>  // for ssize_t

#include <fcntl.h>  // for fcntl
#include <limits.h> // for SSIZE_MAX
#include <netdb.h>  // for getaddrinfo
#include <stddef.h> // for size_t
#include <unistd.h> // for close

// Testable minimk_socket_create implementation.
template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(minimk_errno_get) __minimk_errno_get = minimk_errno_get,
          decltype(socket) __sys_socket = socket>
minimk_error_t __minimk_socket_create(minimk_socket_t *sock, int domain, int type, int protocol) noexcept {
    __minimk_errno_clear();
    int fdesc = __sys_socket(domain, type, protocol);
    int ok = (fdesc >= 0);
    *sock = ok ? (minimk_socket_t)fdesc : -1;
    return ok ? 0 : __minimk_errno_get();
}

// Testable minimk_socket_setnonblock implementation.
template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(minimk_errno_get) __minimk_errno_get = minimk_errno_get,
          decltype(fcntl) __sys_fcntl2 = fcntl,
          decltype(fcntl) __sys_fcntl3 = fcntl>
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

// Testable minimk_socket_connect implementation.
template <decltype(getaddrinfo) __libc_getaddrinfo = getaddrinfo,
          decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(minimk_errno_get) __minimk_errno_get = minimk_errno_get,
          decltype(connect) __sys_connect = connect>
minimk_error_t __minimk_socket_connect(minimk_socket_t sock, const char *address, const char *port) noexcept {
    // Use getaddrinfo to obtain a sockaddr_storage
    addrinfo hints{};
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_NUMERICHOST | AI_NUMERICSERV;
    addrinfo *rp = nullptr;
    if (__libc_getaddrinfo(address, port, &hints, &rp) != 0) {
        return MINIMK_EINVAL;
    }

    // Issue the connect system call
    __minimk_errno_clear();
    int rv = __sys_connect(sock, rp->ai_addr, rp->ai_addrlen);
    freeaddrinfo(rp);

    // Handle the return value
    return (rv == 0) ? 0 : __minimk_errno_get();
}

// Testable minimk_socket_bind implementation.
template <decltype(getaddrinfo) __libc_getaddrinfo = getaddrinfo,
          decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(minimk_errno_get) __minimk_errno_get = minimk_errno_get,
          decltype(bind) __sys_bind = bind>
minimk_error_t __minimk_socket_bind(minimk_socket_t sock, const char *address, const char *port) noexcept {
    // Use getaddrinfo to obtain a sockaddr_storage
    addrinfo hints{};
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_NUMERICHOST | AI_NUMERICSERV;
    addrinfo *rp = nullptr;
    if (__libc_getaddrinfo(address, port, &hints, &rp) != 0) {
        return MINIMK_EINVAL;
    }

    // Issue the bind system call
    __minimk_errno_clear();
    int rv = __sys_bind(sock, rp->ai_addr, rp->ai_addrlen);
    freeaddrinfo(rp);

    // Handle the return value
    return (rv == 0) ? 0 : __minimk_errno_get();
}

// Testable minimk_socket_recv implementation.
template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(minimk_errno_get) __minimk_errno_get = minimk_errno_get,
          decltype(recv) __sys_recv = recv>
minimk_error_t __minimk_socket_recv(minimk_socket_t sock, void *data, size_t count, size_t *nread) noexcept {
    __minimk_errno_clear();
    count = (count <= SSIZE_MAX) ? count : SSIZE_MAX;
    ssize_t rv = __sys_recv((int)sock, data, count, MSG_NOSIGNAL);
    if (rv == -1) {
        *nread = 0;
        return __minimk_errno_get();
    }
    *nread = (size_t)rv;
    return 0;
}

// Testable minimk_socket_send implementation.
template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(minimk_errno_get) __minimk_errno_get = minimk_errno_get,
          decltype(send) __sys_send = send>
minimk_error_t __minimk_socket_send(minimk_socket_t sock, const void *data, size_t count, size_t *nwritten) noexcept {
    __minimk_errno_clear();
    count = (count <= SSIZE_MAX) ? count : SSIZE_MAX;
    ssize_t rv = __sys_send((int)sock, data, count, MSG_NOSIGNAL);
    if (rv == -1) {
        *nwritten = 0;
        return __minimk_errno_get();
    }
    *nwritten = (size_t)rv;
    return 0;
}

// Testable minimk_socket_destroy implementation.
template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(minimk_errno_get) __minimk_errno_get = minimk_errno_get,
          decltype(close) __sys_close = close>
minimk_error_t __minimk_socket_destroy(minimk_socket_t *sock) noexcept {
    __minimk_errno_clear();
    int rc = __sys_close((int)*sock);
    *sock = -1;
    return (rc == 0) ? 0 : __minimk_errno_get();
}

#endif // LIBMINIMK_SOCKET_SOCKET_LINUX_HPP
