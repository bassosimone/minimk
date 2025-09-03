// File: libminimk/socket/set_write_timeout.hpp
// Purpose: set_write_timeout implementation
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SOCKET_SET_WRITE_TIMEOUT_HPP
#define LIBMINIMK_SOCKET_SET_WRITE_TIMEOUT_HPP

#include "../cast/static.hpp" // for CAST_ULL

#include "info.hpp" // for struct socket_info

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_t
#include <minimk/trace.h>  // for MINIMK_TRACE_SOCKET

#include <stdint.h> // for UINT64_MAX

/// Testable minimk_socket_set_write_timeout implementation.
template <decltype(minimk_socket_info_find) M_info_find = minimk_socket_info_find>
minimk_error_t minimk_socket_set_write_timeout_impl(minimk_socket_t sock, uint64_t nanosec) noexcept {
    MINIMK_TRACE_SOCKET("set_write_timeout handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("set_write_timeout nanosec=%llu\n", CAST_ULL(nanosec));

    socket_info *info = nullptr;
    minimk_error_t rv = M_info_find(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("set_write_timeout result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    info->write_timeout = nanosec;
    MINIMK_TRACE_SOCKET("set_write_timeout result=%s\n", minimk_errno_name(0));
    return 0;
}

#endif // LIBMINIMK_SOCKET_SET_WRITE_TIMEOUT_HPP
