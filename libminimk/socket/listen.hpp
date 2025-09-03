// File: libminimk/socket/listen.hpp
// Purpose: listen implementation
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SOCKET_LISTEN_HPP
#define LIBMINIMK_SOCKET_LISTEN_HPP

#include "../cast/static.hpp" // for CAST_ULL

#include "info.hpp" // for struct socket_info

#include <minimk/cdefs.h>   // for MINIMK_ALWAYS_INLINE
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/socket.h>  // for minimk_socket_t
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/trace.h>   // for MINIMK_TRACE_SOCKET

/// Testable minimk_socket_listen implementation.
template <decltype(minimk_socket_info_find) M_info_find = minimk_socket_info_find,
          decltype(minimk_syscall_listen) M_listen = minimk_syscall_listen>
MINIMK_ALWAYS_INLINE minimk_error_t minimk_socket_listen_impl(minimk_socket_t sock, int backlog) noexcept {
    MINIMK_TRACE_SOCKET("listen handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("listen backlog=%d\n", backlog);

    socket_info *info = nullptr;
    minimk_error_t rv = M_info_find(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("listen result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    MINIMK_TRACE_SOCKET("listen fd=%llu\n", CAST_ULL(info->fd));
    rv = M_listen(info->fd, backlog);

    MINIMK_TRACE_SOCKET("listen result=%s\n", minimk_errno_name(rv));
    return rv;
}

#endif // LIBMINIMK_SOCKET_LISTEN_HPP
