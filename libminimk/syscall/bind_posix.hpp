// File: libminimk/syscall/bind_posix.hpp
// Purpose: bind(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_BIND_POSIX_HPP
#define LIBMINIMK_SYSCALL_BIND_POSIX_HPP

#include "errno.h"  // for minimk_syscall_geterrno
#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/errno.h> // for minimk_error_t

#include <sys/socket.h> // for bind

#include <netdb.h> // for getaddrinfo

/// Testable minimk_syscall_bind implementation.
template <decltype(getaddrinfo) libc_getaddrinfo__ = getaddrinfo,
          decltype(minimk_syscall_clearerrno) minimk_syscall_clearerrno__ = minimk_syscall_clearerrno,
          decltype(minimk_syscall_geterrno) minimk_syscall_geterrno__ = minimk_syscall_geterrno,
          decltype(bind) sys_bind__ = bind>
minimk_error_t minimk_syscall_bind__(minimk_syscall_socket_t sock, const char *address,
                                     const char *port) noexcept {
    // Use getaddrinfo to obtain a sockaddr_storage
    addrinfo hints{};
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_NUMERICHOST | AI_NUMERICSERV;
    addrinfo *rp = nullptr;
    if (libc_getaddrinfo__(address, port, &hints, &rp) != 0) {
        return MINIMK_EINVAL;
    }

    // Issue the bind system call
    minimk_syscall_clearerrno__();
    int rv = sys_bind__(sock, rp->ai_addr, rp->ai_addrlen);
    freeaddrinfo(rp);

    // Handle the return value
    return (rv == 0) ? 0 : minimk_syscall_geterrno__();
}

#endif // LIBMINIMK_SYSCALL_BIND_POSIX_HPP
