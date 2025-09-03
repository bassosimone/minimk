// File: libminimk/socket/setsockopt_reuseaddr.hpp
// Purpose: setsockopt_reuseraddr implementation
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SOCKET_SETSOCKOPT_REUSEADDR_HPP
#define LIBMINIMK_SOCKET_SETSOCKOPT_REUSEADDR_HPP

#include "../cast/static.hpp" // for CAST_ULL

#include "info.hpp" // for struct socket_info

#include <minimk/cdefs.h>   // for MINIMK_ALWAYS_INLINE
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/socket.h>  // for minimk_socket_t
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/trace.h>   // for MINIMK_TRACE_SOCKET

/// Testable minimk_socket_setsockopt_reuseaddr implementation.
template <decltype(minimk_socket_info_find) M_info_find = minimk_socket_info_find,
          decltype(minimk_syscall_setsockopt_reuseaddr) M_setsockopt_reuseaddr =
                  minimk_syscall_setsockopt_reuseaddr>
MINIMK_ALWAYS_INLINE minimk_error_t minimk_socket_setsockopt_reuseaddr_impl(minimk_socket_t sock) noexcept {
    MINIMK_TRACE_SOCKET("setsockopt_reuseaddr handle=0x%llx\n", CAST_ULL(sock));

    // Find the corresponding info
    socket_info *info = nullptr;
    minimk_error_t rv = M_info_find(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("setsockopt_reuseaddr result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    // Set SO_REUSEADDR on the underlying socket
    rv = M_setsockopt_reuseaddr(info->fd);
    MINIMK_TRACE_SOCKET("setsockopt_reuseaddr result=%s\n", minimk_errno_name(rv));
    return rv;
}

#endif // LIBMINIMK_SOCKET_SETSOCKOPT_REUSEADDR_HPP
