// File: libminimk/syscall/bind_posix.hpp
// Purpose: bind(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_BIND_POSIX_HPP
#define LIBMINIMK_SYSCALL_BIND_POSIX_HPP

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_geterrno

#include <sys/socket.h> // for bind

#include <netdb.h> // for getaddrinfo

/// Testable minimk_syscall_bind implementation.
template <
        decltype(getaddrinfo) M_libc_getaddrinfo = getaddrinfo,
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
        decltype(bind) M_sys_bind = bind>
minimk_error_t minimk_syscall_bind_impl(minimk_syscall_socket_t sock, const char *address,
                                        const char *port) noexcept {
    // Use getaddrinfo to obtain a sockaddr_storage
    addrinfo hints{};
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_NUMERICHOST | AI_NUMERICSERV;
    addrinfo *rp = nullptr;
    if (M_libc_getaddrinfo(address, port, &hints, &rp) != 0) {
        return MINIMK_EINVAL;
    }

    // Issue the bind system call
    M_minimk_syscall_clearerrno();
    int rv = M_sys_bind(sock, rp->ai_addr, rp->ai_addrlen);
    freeaddrinfo(rp);

    // Handle the return value
    return (rv == 0) ? 0 : M_minimk_syscall_geterrno();
}

#endif // LIBMINIMK_SYSCALL_BIND_POSIX_HPP
