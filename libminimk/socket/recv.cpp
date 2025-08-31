// File: libminimk/socket/recv.cpp
// Purpose: recv implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../cast/static.hpp" // for CAST_ULL

#include "info.hpp" // for struct socket_info

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/runtime.h> // for minimk_runtime_suspend_*
#include <minimk/socket.h>  // for minimk_socket_t
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/trace.h>   // for MINIMK_TRACE_SOCKET

#include <stddef.h> // for size_t

minimk_error_t minimk_socket_recv(minimk_socket_t sock, void *data, size_t count, size_t *nread) noexcept {
    MINIMK_TRACE_SOCKET("recv handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("recv count=%zu\n", count);

    // Find the corresponding info
    socket_info *info = nullptr;
    minimk_error_t rv = minimk_socket_info_find(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("recv result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    MINIMK_TRACE_SOCKET("recv fd=%llu\n", CAST_ULL(info->fd));
    MINIMK_TRACE_SOCKET("recv read_timeout=%llu\n", CAST_ULL(info->read_timeout));

    for (;;) {
        // Attempt to read data
        *nread = 0;
        rv = minimk_syscall_recv(info->fd, data, count, nread);

        MINIMK_TRACE_SOCKET("recv syscall_result=%s\n", minimk_errno_name(rv));
        MINIMK_TRACE_SOCKET("recv nread=%zu\n", *nread);

        // We only need to continue trying on EAGAIN
        if (rv != MINIMK_EAGAIN) {
            MINIMK_TRACE_SOCKET("recv result=%s\n", minimk_errno_name(rv));
            return rv;
        }

        // Block until reading would not block
        MINIMK_TRACE_SOCKET("recv suspend_read fd=%llu\n", CAST_ULL(info->fd));
        MINIMK_TRACE_SOCKET("recv suspend_read timeout=%llu\n", CAST_ULL(info->read_timeout));
        rv = minimk_runtime_suspend_read(info->fd, info->read_timeout);
        if (rv != 0) {
            MINIMK_TRACE_SOCKET("recv suspend_read result=%s\n", minimk_errno_name(rv));
            MINIMK_TRACE_SOCKET("recv result=%s\n", minimk_errno_name(rv));
            return rv;
        }
        MINIMK_TRACE_SOCKET("recv resumed fd=%llu\n", CAST_ULL(info->fd));
    }
}
