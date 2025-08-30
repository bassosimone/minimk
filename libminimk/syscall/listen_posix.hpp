// File: libminimk/syscall/listen_posix.hpp
// Purpose: listen(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_LISTEN_POSIX_HPP
#define LIBMINIMK_SYSCALL_LISTEN_POSIX_HPP

#include "errno.h"  // for minimk_syscall_geterrno
#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/errno.h> // for minimk_error_t

#include <sys/socket.h> // for listen

/// Testable minimk_syscall_listen implementation.
template <decltype(minimk_syscall_clearerrno) minimk_syscall_clearerrno__ = minimk_syscall_clearerrno,
          decltype(minimk_syscall_geterrno) minimk_syscall_geterrno__ = minimk_syscall_geterrno,
          decltype(listen) sys_listen__ = listen>
minimk_error_t minimk_syscall_listen__(minimk_syscall_socket_t sock, int backlog) noexcept {
    minimk_syscall_clearerrno__();
    int rv = sys_listen__((int)sock, backlog);
    return (rv == 0) ? 0 : minimk_syscall_geterrno__();
}

#endif // LIBMINIMK_SYSCALL_LISTEN_POSIX_HPP
