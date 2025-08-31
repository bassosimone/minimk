// File: libminimk/socket/recvall.cpp
// Purpose: recvall implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../cast/static.hpp" // for CAST_ULL

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/io.hpp>   // for minimk_io_readall
#include <minimk/socket.h> // for minimk_socket_t
#include <minimk/trace.h>  // for MINIMK_TRACE_SOCKET

#include <stddef.h> // for size_t

minimk_error_t minimk_socket_recvall(minimk_socket_t sock, void *buf, size_t count) noexcept {
    MINIMK_TRACE_SOCKET("recvall handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("recvall count=%zu\n", count);
    minimk_error_t rv = minimk_io_readall<minimk_socket_t, minimk_socket_recv>(sock, buf, count);
    MINIMK_TRACE_SOCKET("recvall result=%s\n", minimk_errno_name(rv));
    return rv;
}
