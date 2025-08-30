// File: libminimk/syscall/connect_posix.hpp
// Purpose: connect(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_CONNECT_POSIX_HPP
#define LIBMINIMK_SYSCALL_CONNECT_POSIX_HPP

#include "../errno/errno.h" // for minimk_errno_get

#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/errno.h> // for minimk_errno_clear

#include <sys/socket.h> // for connect

#include <netdb.h> // for getaddrinfo

/// Testable minimk_syscall_connect implementation.
template <decltype(getaddrinfo) libc_getaddrinfo__ = getaddrinfo,
          decltype(minimk_errno_clear) minimk_errno_clear__ = minimk_errno_clear,
          decltype(minimk_errno_get) minimk_errno_get__ = minimk_errno_get,
          decltype(connect) sys_connect__ = connect>
minimk_error_t minimk_syscall_connect__(minimk_syscall_socket_t sock, const char *address,
                                        const char *port) noexcept {
    // Use getaddrinfo to obtain a sockaddr_storage
    addrinfo hints{};
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_NUMERICHOST | AI_NUMERICSERV;
    addrinfo *rp = nullptr;
    if (libc_getaddrinfo__(address, port, &hints, &rp) != 0) {
        return MINIMK_EINVAL;
    }

    // Issue the connect system call
    minimk_errno_clear__();
    int rv = sys_connect__(sock, rp->ai_addr, rp->ai_addrlen);
    freeaddrinfo(rp);

    // Handle the return value
    return (rv == 0) ? 0 : minimk_errno_get__();
}

#endif // LIBMINIMK_SYSCALL_CONNECT_POSIX_HPP
