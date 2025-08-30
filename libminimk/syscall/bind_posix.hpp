// File: libminimk/syscall/bind_posix.hpp
// Purpose: bind(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_BIND_POSIX_HPP
#define LIBMINIMK_SYSCALL_BIND_POSIX_HPP

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_geterrno
#include <minimk/trace.h>   // for MINIMK_TRACE_SYSCALL

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
        MINIMK_TRACE_SYSCALL("bind: getaddrinfo failed for %s:%s\n", address, port);
        return MINIMK_EINVAL;
    }

    // Log that we're about to invoke the syscall
    MINIMK_TRACE_SYSCALL("bind: fd=%d\n", sock);
    MINIMK_TRACE_SYSCALL("bind: address=%s\n", address);
    MINIMK_TRACE_SYSCALL("bind: port=%s\n", port);

    // Clear the errno and issue the syscall
    M_minimk_syscall_clearerrno();
    int rv = M_sys_bind(sock, rp->ai_addr, rp->ai_addrlen);
    freeaddrinfo(rp);

    // Assign the result of the syscall
    minimk_error_t res = (rv == 0) ? 0 : M_minimk_syscall_geterrno();

    // Log the results of invoking the syscall
    MINIMK_TRACE_SYSCALL("bind: result=%s\n", minimk_errno_name(res));

    // Return the result
    return res;
}

#endif // LIBMINIMK_SYSCALL_BIND_POSIX_HPP
