// File: libminimk/socket/connect.cpp
// Purpose: connect implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../cast/static.hpp" // for CAST_ULL

#include "info.hpp" // for struct socket_info

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/runtime.h> // for minimk_runtime_suspend_*
#include <minimk/socket.h>  // for minimk_socket_t
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/trace.h>   // for MINIMK_TRACE_SOCKET

minimk_error_t minimk_socket_connect(minimk_socket_t sock, const char *address, const char *port) noexcept {
    MINIMK_TRACE_SOCKET("connect handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("connect address=%s\n", address);
    MINIMK_TRACE_SOCKET("connect port=%s\n", port);

    // Find the corresponding info
    socket_info *info = nullptr;
    minimk_error_t rv = minimk_socket_info_find(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("connect result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    MINIMK_TRACE_SOCKET("connect fd=%llu\n", CAST_ULL(info->fd));
    MINIMK_TRACE_SOCKET("connect write_timeout=%llu\n", CAST_ULL(info->write_timeout));

    // Attempt non-blocking connect
    rv = minimk_syscall_connect(info->fd, address, port);
    MINIMK_TRACE_SOCKET("connect syscall_result=%s\n", minimk_errno_name(rv));

    // If connect succeeded immediately, we're done
    if (rv == 0) {
        MINIMK_TRACE_SOCKET("connect result=%s\n", minimk_errno_name(0));
        return 0;
    }

    // Distinguish between the "in progress" signal and a real error
#ifdef _WIN32
    bool inprogress = (rv == MINIMK_EINPROGRESS || rv == MINIMK_EWOULDBLOCK);
#else
    bool inprogress = (rv == MINIMK_EINPROGRESS);
#endif
    if (!inprogress) {
        MINIMK_TRACE_SOCKET("connect result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    // Wait for socket to become writeable (connection in progress)
    MINIMK_TRACE_SOCKET("connect suspend_write fd=%llu\n", CAST_ULL(info->fd));
    MINIMK_TRACE_SOCKET("connect suspend_write timeout=%llu\n", CAST_ULL(info->write_timeout));
    rv = minimk_runtime_suspend_write(info->fd, info->write_timeout);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("connect suspend_write result=%s\n", minimk_errno_name(rv));
        MINIMK_TRACE_SOCKET("connect result=%s\n", minimk_errno_name(rv));
        return rv;
    }
    MINIMK_TRACE_SOCKET("connect resumed fd=%llu\n", CAST_ULL(info->fd));

    // Check SO_ERROR to determine connection result
    minimk_error_t connect_error = 0;
    rv = minimk_syscall_getsockopt_error(info->fd, &connect_error);
    MINIMK_TRACE_SOCKET("connect getsockopt_error result=%s\n", minimk_errno_name(rv));
    MINIMK_TRACE_SOCKET("connect getsockopt_error connect_error=%s\n", minimk_errno_name(connect_error));

    if (rv != 0) {
        MINIMK_TRACE_SOCKET("connect result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    // Return the actual connection result
    MINIMK_TRACE_SOCKET("connect result=%s\n", minimk_errno_name(connect_error));
    return connect_error;
}
