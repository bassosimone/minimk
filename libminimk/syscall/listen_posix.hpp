// File: libminimk/syscall/listen_posix.hpp
// Purpose: listen(2) on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_LISTEN_POSIX_HPP
#define LIBMINIMK_SYSCALL_LISTEN_POSIX_HPP

#include <minimk/cdefs.h>   // for MINIMK_ALWAYS_INLINE
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_geterrno
#include <minimk/trace.h>   // for MINIMK_TRACE_SYSCALL

#include <sys/socket.h> // for listen

/// Testable minimk_syscall_listen implementation.
template <decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
          decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
          decltype(listen) M_sys_listen = listen>
MINIMK_ALWAYS_INLINE minimk_error_t minimk_syscall_listen_impl(minimk_syscall_socket_t sock,
                                                               int backlog) noexcept {
    // Log that we're about to invoke the syscall
    MINIMK_TRACE_SYSCALL("listen: fd=%d\n", sock);
    MINIMK_TRACE_SYSCALL("listen: backlog=%d\n", backlog);

    // Clear the errno and issue the syscall
    M_minimk_syscall_clearerrno();
    int rv = M_sys_listen(sock, backlog);

    // Assign the result of the syscall
    minimk_error_t res = (rv == 0) ? 0 : M_minimk_syscall_geterrno();

    // Log the results of invoking the syscall
    MINIMK_TRACE_SYSCALL("listen: result=%s\n", minimk_errno_name(res));

    // Return the result
    return res;
}

#endif // LIBMINIMK_SYSCALL_LISTEN_POSIX_HPP
