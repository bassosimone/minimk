// File: libminimk/socket/socket.cpp
// Purpose: runtime-managed socket table with ECS-style resource management
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../cast/static.hpp" // for CAST_ULL

#include "handle.hpp" // for make_handle

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/io.hpp>    // for minimk_io_readall
#include <minimk/runtime.h> // for minimk_runtime_suspend_*
#include <minimk/socket.h>  // for minimk_socket_t
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/trace.h>   // for MINIMK_TRACE_SOCKET

#include <stddef.h> // for size_t
#include <stdint.h> // for UINT64_MAX

/// Maximum number of sockets managed by the runtime.
#define MAX_SOCKETS MAX_HANDLES

/// Socket slot in the runtime table.
struct socketinfo {
    /// The actual handle associated to this entry.
    minimk_socket_t handle;

    /// Read timeout in nanoseconds.
    uint64_t read_timeout;

    /// Write timeout in nanoseconds.
    uint64_t write_timeout;

    /// The underlying OS socket file descriptor.
    minimk_syscall_socket_t fd;

    /// Padding to align to 16 bytes.
    uint32_t padding;
};

/// Global socket table managed by the runtime.
static socketinfo sockets[MAX_SOCKETS];

/// Global generation counter - incremented each time we wrap around the table.
static uint64_t generation = 0;

/// Next slot to try allocating from.
static size_t next_slot = 0;

/// Common function for accessing the sockets array.
static socketinfo *get_socketinfo(size_t idx) noexcept {
    MINIMK_ASSERT(idx >= 0 && idx < MAX_SOCKETS);
#pragma clang unsafe_buffer_usage begin
    return &sockets[idx];
#pragma clang unsafe_buffer_usage end
}

/// Find and validate socket info from ID, returning nullptr if invalid.
static minimk_error_t find_socketinfo(socketinfo **pinfo, minimk_socket_t handle) noexcept {
    // Zero the return argument
    *pinfo = nullptr;

    MINIMK_TRACE_SOCKET("find_socketinfo handle=0x%llx\n", CAST_ULL(handle));

    // Unpack the handle into its components
    uint8_t type = handle_type(handle);
    MINIMK_TRACE_SOCKET("find_socketinfo type=0x%llx\n", CAST_ULL(type));
    MINIMK_TRACE_SOCKET("find_socketinfo generation=0x%llx\n", CAST_ULL(handle_generation(handle)));
    uint64_t index = handle_index(handle);
    MINIMK_TRACE_SOCKET("find_socketinfo index=0x%llx\n", CAST_ULL(index));

    // Reject handles owned by other subsystems
    if (type != HANDLE_TYPE_SOCKET) {
        MINIMK_TRACE_SOCKET("find_socketinfo result=%s\n", minimk_errno_name(MINIMK_EBADF));
        return MINIMK_EBADF;
    }

    // Access the corresponding slot
    socketinfo *info = get_socketinfo(index);
    MINIMK_TRACE_SOCKET("find_socketinfo ptr=%p\n", CAST_VOID_P(info));
    MINIMK_TRACE_SOCKET("find_socketinfo stored_handle=0x%llx\n", CAST_ULL(info->handle));

    // Ensure the handle is actually correct
    if (info->handle != handle) {
        MINIMK_TRACE_SOCKET("find_socketinfo result=%s\n", minimk_errno_name(MINIMK_EBADF));
        return MINIMK_EBADF;
    }

    MINIMK_TRACE_SOCKET("find_socketinfo %s\n", "generation match");
    *pinfo = info;
    MINIMK_TRACE_SOCKET("find_socketinfo result=%s\n", minimk_errno_name(0));
    return 0;
}

/// Create socketinfo for an existing socket fd.
static minimk_error_t create_socketinfo(socketinfo **pinfo, minimk_syscall_socket_t fd) noexcept {
    // Zero the return argument
    *pinfo = nullptr;

    MINIMK_TRACE_SOCKET("create_socketinfo next_slot=%zu\n", next_slot);
    MINIMK_TRACE_SOCKET("create_socketinfo generation=%llu\n", CAST_ULL(generation));

    // We need to search at most MAX_SOCKETS times before giving up
    for (size_t idx = 0; idx < MAX_SOCKETS && *pinfo == nullptr; idx++) {
        // Use modulo to stay within the bounds
        size_t slot_index = next_slot % MAX_SOCKETS;
        socketinfo *info = get_socketinfo(slot_index);

        MINIMK_TRACE_SOCKET("create_socketinfo iteration=%zu\n", idx);
        MINIMK_TRACE_SOCKET("create_socketinfo candidate_index=%zu\n", slot_index);
        MINIMK_TRACE_SOCKET("create_socketinfo next_slot=%zu\n", next_slot);
        MINIMK_TRACE_SOCKET("create_socketinfo stored_handle=0x%llx\n", CAST_ULL(info->handle));

        // A zero handle indicates that the slot is free
        if (info->handle == 0) {
            MINIMK_TRACE_SOCKET("create_socketinfo %s\n", "slot is free");

            *pinfo = info;
            MINIMK_ASSERT(slot_index <= UINT8_MAX);
            info->handle = make_handle(HANDLE_TYPE_SOCKET, generation, static_cast<uint8_t>(slot_index));
            info->fd = fd;
            info->read_timeout = UINT64_MAX;
            info->write_timeout = UINT64_MAX;

            MINIMK_TRACE_SOCKET("create_socketinfo type=0x%llx\n", CAST_ULL(HANDLE_TYPE_SOCKET));
            MINIMK_TRACE_SOCKET("create_socketinfo generation=0x%llx\n", CAST_ULL(generation));
            MINIMK_TRACE_SOCKET("create_socketinfo index=0x%llx\n", CAST_ULL(slot_index));
            MINIMK_TRACE_SOCKET("create_socketinfo handle=0x%llx\n", CAST_ULL(info->handle));
            // FALLTHROUGH
        }

        // Increment the next slot to search
        next_slot++;

        // Check for a jump to the next generation
        if (next_slot >= 2 && (next_slot % MAX_SOCKETS) == 0) {
            MINIMK_TRACE_SOCKET("create_socketinfo %s", "generation++\n");
            generation++;
        }
    }

    // Return the appropriate result
    if (*pinfo == nullptr) {
        MINIMK_TRACE_SOCKET("create_socketinfo result=%s\n", minimk_errno_name(MINIMK_EMFILE));
        return MINIMK_EMFILE;
    }
    MINIMK_TRACE_SOCKET("create_socketinfo result=%s\n", minimk_errno_name(0));
    return 0;
}

/// Destroy socketinfo and free the slot.
static void destroy_socketinfo(socketinfo *info) noexcept {
    *info = {};
}

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

    socketinfo *info = nullptr;
    rv = create_socketinfo(&info, sockfd);
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

minimk_error_t minimk_socket_bind(minimk_socket_t sock, const char *address, const char *port) noexcept {
    MINIMK_TRACE_SOCKET("bind handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("bind address=%s\n", address);
    MINIMK_TRACE_SOCKET("bind port=%s\n", port);

    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("bind result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    MINIMK_TRACE_SOCKET("bind fd=%llu\n", CAST_ULL(info->fd));
    rv = minimk_syscall_bind(info->fd, address, port);

    MINIMK_TRACE_SOCKET("bind result=%s\n", minimk_errno_name(rv));
    return rv;
}

minimk_error_t minimk_socket_listen(minimk_socket_t sock, int backlog) noexcept {
    MINIMK_TRACE_SOCKET("listen handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("listen backlog=%d\n", backlog);

    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("listen result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    MINIMK_TRACE_SOCKET("listen fd=%llu\n", CAST_ULL(info->fd));
    rv = minimk_syscall_listen(info->fd, backlog);

    MINIMK_TRACE_SOCKET("listen result=%s\n", minimk_errno_name(rv));
    return rv;
}

minimk_error_t minimk_socket_accept(minimk_socket_t *client_sock, minimk_socket_t listener_sock) noexcept {
    MINIMK_TRACE_SOCKET("accept listenerfd=0x%llx\n", CAST_ULL(listener_sock));

    // Invalidate the handle, as documented
    *client_sock = MINIMK_SOCKET_INVALID;

    // Find the corresponding info
    socketinfo *listener_info = nullptr;
    minimk_error_t rv = find_socketinfo(&listener_info, listener_sock);
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
        socketinfo *client_info = nullptr;
        rv = create_socketinfo(&client_info, client_fd);
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

minimk_error_t minimk_socket_recv(minimk_socket_t sock, void *data, size_t count, size_t *nread) noexcept {
    MINIMK_TRACE_SOCKET("recv handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("recv count=%zu\n", count);

    // Find the corresponding info
    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, sock);
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

minimk_error_t minimk_socket_send(minimk_socket_t sock, const void *data, size_t count,
                                  size_t *nwritten) noexcept {
    MINIMK_TRACE_SOCKET("send handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("send count=%zu\n", count);

    // Find the corresponding info
    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, sock);
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

minimk_error_t minimk_socket_setsockopt_reuseaddr(minimk_socket_t sock) noexcept {
    MINIMK_TRACE_SOCKET("setsockopt_reuseaddr handle=0x%llx\n", CAST_ULL(sock));

    // Find the corresponding info
    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("setsockopt_reuseaddr result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    // Set SO_REUSEADDR on the underlying socket
    rv = minimk_syscall_setsockopt_reuseaddr(info->fd);
    MINIMK_TRACE_SOCKET("setsockopt_reuseaddr result=%s\n", minimk_errno_name(rv));
    return rv;
}

minimk_error_t minimk_socket_destroy(minimk_socket_t *sock) noexcept {
    MINIMK_TRACE_SOCKET("destroy handle=0x%llx\n", CAST_ULL(*sock));

    // Idempotent destroy
    if (*sock == MINIMK_SOCKET_INVALID) {
        MINIMK_TRACE_SOCKET("destroy result=%s\n", minimk_errno_name(0));
        return 0;
    }

    // Find the corresponding info
    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, *sock);
    if (rv != 0) {
        *sock = MINIMK_SOCKET_INVALID;
        MINIMK_TRACE_SOCKET("destroy result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    MINIMK_TRACE_SOCKET("destroy fd=%llu\n", CAST_ULL(info->fd));

    // Close the OS socket
    rv = minimk_syscall_closesocket(&info->fd);

    // Clear the slot
    destroy_socketinfo(info);

    // Invalidate the caller's handle
    *sock = MINIMK_SOCKET_INVALID;

    // Return the closing error
    MINIMK_TRACE_SOCKET("destroy result=%s\n", minimk_errno_name(rv));
    return rv;
}

minimk_error_t minimk_socket_sendall(minimk_socket_t sock, const void *buf, size_t count) noexcept {
    MINIMK_TRACE_SOCKET("sendall handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("sendall count=%zu\n", count);
    minimk_error_t rv = minimk_io_writeall<minimk_socket_t, minimk_socket_send>(sock, buf, count);
    MINIMK_TRACE_SOCKET("sendall result=%s\n", minimk_errno_name(rv));
    return rv;
}

minimk_error_t minimk_socket_set_read_timeout(minimk_socket_t sock, uint64_t nanosec) noexcept {
    MINIMK_TRACE_SOCKET("set_read_timeout handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("set_read_timeout nanosec=%llu\n", CAST_ULL(nanosec));

    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("set_read_timeout result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    info->read_timeout = nanosec;
    MINIMK_TRACE_SOCKET("set_read_timeout result=%s\n", minimk_errno_name(0));
    return 0;
}

minimk_error_t minimk_socket_set_write_timeout(minimk_socket_t sock, uint64_t nanosec) noexcept {
    MINIMK_TRACE_SOCKET("set_write_timeout handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("set_write_timeout nanosec=%llu\n", CAST_ULL(nanosec));

    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE_SOCKET("set_write_timeout result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    info->write_timeout = nanosec;
    MINIMK_TRACE_SOCKET("set_write_timeout result=%s\n", minimk_errno_name(0));
    return 0;
}

minimk_error_t minimk_socket_recvall(minimk_socket_t sock, void *buf, size_t count) noexcept {
    MINIMK_TRACE_SOCKET("recvall handle=0x%llx\n", CAST_ULL(sock));
    MINIMK_TRACE_SOCKET("recvall count=%zu\n", count);
    minimk_error_t rv = minimk_io_readall<minimk_socket_t, minimk_socket_recv>(sock, buf, count);
    MINIMK_TRACE_SOCKET("recvall result=%s\n", minimk_errno_name(rv));
    return rv;
}
