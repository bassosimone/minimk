// File: libminimk/socket/send.cpp
// Purpose: send implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../cast/static.hpp" // for CAST_ULL

#include "info.hpp" // for struct socket_info

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/runtime.h> // for minimk_runtime_suspend_*
#include <minimk/socket.h>  // for minimk_socket_t
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/trace.h>   // for MINIMK_TRACE_SOCKET

#include <stddef.h> // for size_t

minimk_error_t minimk_socket_send(minimk_socket_t sock, const void *data, size_t count,
                                  size_t *nwritten) noexcept {
    MINIMK_TRACE_SOCKET("send handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("send count=%zu\n", count);

    // Find the corresponding info
    socket_info *info = nullptr;
    minimk_error_t rv = minimk_socket_info_find(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("send result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    MINIMK_TRACE_SOCKET("send fd=%llu\n", CAST_ULL(info->fd));
    MINIMK_TRACE_SOCKET("send write_timeout=%llu\n", CAST_ULL(info->write_timeout));

    for (;;) {
        // Attempt to send data
        *nwritten = 0;
        rv = minimk_syscall_send(info->fd, data, count, nwritten);

        MINIMK_TRACE_SOCKET("send syscall_result=%s\n", minimk_errno_name(rv));
        MINIMK_TRACE_SOCKET("send nwritten=%zu\n", *nwritten);

        // We only need to continue trying on EAGAIN
        if (rv != MINIMK_EAGAIN) {
            MINIMK_TRACE_SOCKET("send result=%s\n", minimk_errno_name(rv));
            return rv;
        }

        // Block until ready
        MINIMK_TRACE_SOCKET("send suspend_write fd=%llu\n", CAST_ULL(info->fd));
        MINIMK_TRACE_SOCKET("send suspend_write timeout=%llu\n", CAST_ULL(info->write_timeout));
        rv = minimk_runtime_suspend_write(info->fd, info->write_timeout);
        if (rv != 0) {
            MINIMK_TRACE_SOCKET("send suspend_write result=%s\n", minimk_errno_name(rv));
            MINIMK_TRACE_SOCKET("send result=%s\n", minimk_errno_name(rv));
            return rv;
        }
        MINIMK_TRACE_SOCKET("send resumed fd=%llu\n", CAST_ULL(info->fd));
    }
}
