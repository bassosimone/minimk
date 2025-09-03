// File: libminimk/socket/sendall.hpp
// Purpose: sendall implementation
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SOCKET_SENDALL_HPP
#define LIBMINIMK_SOCKET_SENDALL_HPP

#include "../cast/static.hpp" // for CAST_ULL

#include <minimk/cdefs.h>  // for MINIMK_ALWAYS_INLINE
#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/io.hpp>   // for minimk_io_writeall
#include <minimk/socket.h> // for minimk_socket_t
#include <minimk/trace.h>  // for MINIMK_TRACE_SOCKET

#include <stddef.h> // for size_t

/// Testable minimk_socket_sendall implementation.
template <decltype(minimk_socket_send) M_send = minimk_socket_send>
MINIMK_ALWAYS_INLINE minimk_error_t minimk_socket_sendall_impl(minimk_socket_t sock, const void *buf,
                                                               size_t count) noexcept {
    MINIMK_TRACE_SOCKET("sendall handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("sendall count=%zu\n", count);
    minimk_error_t rv = minimk_io_writeall<minimk_socket_t, M_send>(sock, buf, count);
    MINIMK_TRACE_SOCKET("sendall result=%s\n", minimk_errno_name(rv));
    return rv;
}

#endif // LIBMINIMK_SOCKET_SENDALL_HPP
