// File: libminimk/socket/create.cpp
// Purpose: create implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../cast/static.hpp" // for CAST_ULL

#include "info.hpp" // for struct socket_info

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/socket.h>  // for minimk_socket_t
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/trace.h>   // for MINIMK_TRACE_SOCKET

minimk_error_t minimk_socket_create(minimk_socket_t *sock, int domain, int type, int protocol) noexcept {
    // Invalidate the handle, as documented
    *sock = MINIMK_SOCKET_INVALID;

    // Create the underlying socket
    MINIMK_TRACE_SOCKET("socket domain=%d type=%d protocol=%d\n", domain, type, protocol);

    minimk_syscall_socket_t sockfd = minimk_syscall_invalid_socket;
    minimk_error_t rv = minimk_syscall_socket(&sockfd, domain, type, protocol);
    MINIMK_TRACE_SOCKET("socket result=%s\n", minimk_errno_name(rv));
    MINIMK_TRACE_SOCKET("socket fd=%llu\n", CAST_ULL(sockfd));

    if (rv != 0) {
        return rv;
    }

    // Make it nonblocking
    MINIMK_TRACE_SOCKET("setnonblock fd=%llu\n", CAST_ULL(sockfd));

    rv = minimk_syscall_socket_setnonblock(sockfd);
    MINIMK_TRACE_SOCKET("setnonblock result=%s\n", minimk_errno_name(rv));

    if (rv != 0) {
        minimk_syscall_closesocket(&sockfd);
        return rv;
    }

    // Set SO_NOSIGPIPE when available (FreeBSD/macOS) for defense-in-depth SIGPIPE prevention
    MINIMK_TRACE_SOCKET("setsockopt_nosigpipe fd=%llu\n", CAST_ULL(sockfd));

    rv = minimk_syscall_setsockopt_nosigpipe(sockfd);
    MINIMK_TRACE_SOCKET("setsockopt_nosigpipe result=%s\n", minimk_errno_name(rv));

    // Note: we don't fail socket creation if SO_NOSIGPIPE fails since:
    // 1. It's not available on all platforms (Linux doesn't have it)
    // 2. We have MSG_NOSIGNAL as primary protection on Linux
    // 3. This is defense-in-depth, not essential functionality

    // Create the corresponding socketinfo
    MINIMK_TRACE_SOCKET("create_socketinfo fd=%llu\n", CAST_ULL(sockfd));

    socket_info *info = nullptr;
    rv = minimk_socket_info_create(&info, sockfd);
    MINIMK_TRACE_SOCKET("create_socketinfo result=%s\n", minimk_errno_name(rv));

    if (rv != 0) {
        minimk_syscall_closesocket(&sockfd);
        return rv;
    }

    // Return the socket handle to the caller
    *sock = info->handle;
    MINIMK_TRACE_SOCKET("socket result=%s\n", minimk_errno_name(0));
    MINIMK_TRACE_SOCKET("socket handle=0x%llx\n", CAST_ULL(*sock));
    return 0;
}
