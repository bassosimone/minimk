// File: libminimk/syscall/setsockopt_nosigpipe_posix.hpp
// Purpose: setsockopt(2) on POSIX for SO_NOSIGPIPE
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SETSOCKOPT_NOSIGPIPE_POSIX_HPP
#define LIBMINIMK_SYSCALL_SETSOCKOPT_NOSIGPIPE_POSIX_HPP

#include "errno.h"  // for minimk_syscall_geterrno
#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/errno.h> // for minimk_error_t

#include <sys/socket.h> // for setsockopt

/// Testable minimk_syscall_setsockopt_nosigpipe implementation.
template <
        decltype(minimk_syscall_clearerrno) minimk_syscall_clearerrno__ = minimk_syscall_clearerrno,
        decltype(minimk_syscall_geterrno) minimk_syscall_geterrno__ = minimk_syscall_geterrno,
        decltype(setsockopt) sys_setsockopt__ = setsockopt>
minimk_error_t minimk_syscall_setsockopt_nosigpipe__(minimk_syscall_socket_t sock) noexcept {
#ifdef SO_NOSIGPIPE
    // Set SO_NOSIGPIPE to avoid SIGPIPE on closed connection writes
    int on = 1;
    minimk_syscall_clearerrno__();
    if (sys_setsockopt__((int)sock, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on)) != 0) {
        return minimk_syscall_geterrno__();
    }
#else
    // SO_NOSIGPIPE is not available on this platform - this is a no-op
    (void)sock; // suppress unused parameter warning
#endif // SO_NOSIGPIPE
    return 0;
}

#endif // LIBMINIMK_SYSCALL_SETSOCKOPT_NOSIGPIPE_POSIX_HPP
