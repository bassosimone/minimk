// File: libminimk/syscall/connect_posix.hpp
// Purpose: connect(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_CONNECT_POSIX_HPP
#define LIBMINIMK_SYSCALL_CONNECT_POSIX_HPP

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_geterrno
#include <minimk/time.h>    // for minimk_time_monotonic_now
#include <minimk/trace.h>   // for MINIMK_TRACE_SYSCALL

#include <sys/socket.h> // for connect

#include <netdb.h> // for getaddrinfo

/// Testable minimk_syscall_connect implementation.
template <
        decltype(getaddrinfo) M_libc_getaddrinfo = getaddrinfo,
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
        decltype(connect) M_sys_connect = connect>
minimk_error_t minimk_syscall_connect_impl(minimk_syscall_socket_t sock, const char *address,
                                           const char *port) noexcept {
    // Use getaddrinfo to obtain a sockaddr_storage
    addrinfo hints{};
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_NUMERICHOST | AI_NUMERICSERV;
    addrinfo *rp = nullptr;
    if (M_libc_getaddrinfo(address, port, &hints, &rp) != 0) {
        MINIMK_TRACE_SYSCALL("connect: getaddrinfo failed for %s:%s\n", address, port);
        return MINIMK_EINVAL;
    }

    // Log that we're about to invoke the syscall
    MINIMK_TRACE_SYSCALL("connect: fd=%d\n", sock);
    MINIMK_TRACE_SYSCALL("connect: address=%s\n", address);
    MINIMK_TRACE_SYSCALL("connect: port=%s\n", port);
    MINIMK_TRACE_SYSCALL("connect: t0=%lu\n", minimk_time_monotonic_now());

    // Clear the errno and issue the syscall
    M_minimk_syscall_clearerrno();
    int rv = M_sys_connect(sock, rp->ai_addr, rp->ai_addrlen);
    freeaddrinfo(rp);

    // Assign the result of the syscall
    minimk_error_t res = (rv == 0) ? 0 : M_minimk_syscall_geterrno();

    // Log the results of invoking the syscall
    MINIMK_TRACE_SYSCALL("connect: result=%s\n", minimk_errno_name(res));
    MINIMK_TRACE_SYSCALL("connect: t1=%lu\n", minimk_time_monotonic_now());

    // Return the result
    return res;
}

#endif // LIBMINIMK_SYSCALL_CONNECT_POSIX_HPP
