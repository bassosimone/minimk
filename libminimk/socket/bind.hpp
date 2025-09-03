// File: libminimk/socket/bind.hpp
// Purpose: bind implementation
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SOCKET_BIND_HPP
#define LIBMINIMK_SOCKET_BIND_HPP

#include "../cast/static.hpp" // for CAST_ULL

#include "info.hpp" // for struct socket_info

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/socket.h>  // for minimk_socket_t
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/trace.h>   // for MINIMK_TRACE_SOCKET

/// Testable minimk_socket_bind implementation.
template <decltype(minimk_socket_info_find) M_info_find = minimk_socket_info_find,
          decltype(minimk_syscall_bind) M_bind = minimk_syscall_bind>
minimk_error_t minimk_socket_bind_impl(minimk_socket_t sock, const char *address, const char *port) noexcept {
    MINIMK_TRACE_SOCKET("bind handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("bind address=%s\n", address);
    MINIMK_TRACE_SOCKET("bind port=%s\n", port);

    socket_info *info = nullptr;
    minimk_error_t rv = M_info_find(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("bind result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    MINIMK_TRACE_SOCKET("bind fd=%llu\n", CAST_ULL(info->fd));
    rv = M_bind(info->fd, address, port);

    MINIMK_TRACE_SOCKET("bind result=%s\n", minimk_errno_name(rv));
    return rv;
}

#endif // LIBMINIMK_SOCKET_BIND_HPP
