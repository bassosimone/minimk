// File: libminimk/socket/accept.cpp
// Purpose: implements accept
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../cast/static.hpp" // for CAST_ULL

#include "info.hpp" // for struct socket_info

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/runtime.h> // for minimk_runtime_suspend_*
#include <minimk/socket.h>  // for minimk_socket_t
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/trace.h>   // for MINIMK_TRACE_SOCKET

minimk_error_t minimk_socket_accept(minimk_socket_t *client_sock, minimk_socket_t listener_sock) noexcept {
    MINIMK_TRACE_SOCKET("accept listenerfd=0x%llx\n", CAST_ULL(listener_sock));

    // Invalidate the handle, as documented
    *client_sock = MINIMK_SOCKET_INVALID;

    // Find the corresponding info
    socket_info *listener_info = nullptr;
    minimk_error_t rv = minimk_socket_info_find(&listener_info, listener_sock);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("accept result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    MINIMK_TRACE_SOCKET("accept fd=%llu\n", CAST_ULL(listener_info->fd));
    MINIMK_TRACE_SOCKET("accept read_timeout=%llu\n", CAST_ULL(listener_info->read_timeout));

    for (;;) {
        // Attempt to accept a connection
        minimk_syscall_socket_t client_fd = minimk_syscall_invalid_socket;
        rv = minimk_syscall_accept(&client_fd, listener_info->fd);

        MINIMK_TRACE_SOCKET("accept syscall_result=%s\n", minimk_errno_name(rv));

        // Suspend if needed
        if (rv == MINIMK_EAGAIN) {
            MINIMK_TRACE_SOCKET("accept suspend_read fd=%llu\n", CAST_ULL(listener_info->fd));
            MINIMK_TRACE_SOCKET("accept suspend_read timeout=%llu\n", CAST_ULL(listener_info->read_timeout));
            rv = minimk_runtime_suspend_read(listener_info->fd, listener_info->read_timeout);
            if (rv != 0) {
                MINIMK_TRACE_SOCKET("accept suspend_read result=%s\n", minimk_errno_name(rv));
                MINIMK_TRACE_SOCKET("accept result=%s\n", minimk_errno_name(rv));
                return rv;
            }
            MINIMK_TRACE_SOCKET("accept resumed fd=%llu\n", CAST_ULL(listener_info->fd));
            continue;
        }

        // Handle any other kind of errors
        if (rv != 0) {
            MINIMK_TRACE_SOCKET("accept result=%s\n", minimk_errno_name(rv));
            return rv;
        }

        // Make socket nonblocking
        MINIMK_TRACE_SOCKET("accept clientfd=%llu\n", CAST_ULL(client_fd));
        rv = minimk_syscall_socket_setnonblock(client_fd);
        if (rv != 0) {
            MINIMK_TRACE_SOCKET("accept result=%s\n", minimk_errno_name(rv));
            minimk_syscall_closesocket(&client_fd);
            return rv;
        }

        // Set SO_NOSIGPIPE when available (FreeBSD/macOS) for defense-in-depth SIGPIPE prevention
        MINIMK_TRACE_SOCKET("accept setsockopt_nosigpipe clientfd=%llu\n", CAST_ULL(client_fd));
        rv = minimk_syscall_setsockopt_nosigpipe(client_fd);
        MINIMK_TRACE_SOCKET("accept setsockopt_nosigpipe result=%s\n", minimk_errno_name(rv));
        // Note: we don't fail on SO_NOSIGPIPE failure (same reasoning as in create)

        // Create the corresponding socketinfo
        socket_info *client_info = nullptr;
        rv = minimk_socket_info_create(&client_info, client_fd);
        if (rv != 0) {
            MINIMK_TRACE_SOCKET("accept result=%s\n", minimk_errno_name(rv));
            minimk_syscall_closesocket(&client_fd);
            return rv;
        }

        // Return the socket handle to the caller
        *client_sock = client_info->handle;
        MINIMK_TRACE_SOCKET("accept result=%s\n", minimk_errno_name(0));
        MINIMK_TRACE_SOCKET("accept client_handle=0x%llx\n", CAST_ULL(*client_sock));
        return 0;
    }
}
