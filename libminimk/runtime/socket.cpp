// File: libminimk/runtime/socket.cpp
// Purpose: runtime-managed socket table with ECS-style resource management
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../io/io.hpp" // for __minimk_io_readall
#include "../poll/poll.h" // for minimk_poll functions
#include "../socket/socket.h"  // for minimk_socket_t and operations
#include "handle.h"     // for __make_handle
#include "runtime.h"    // for minimk_runtime_suspend_read/write
#include "trace.h"      // for MINIMK_TRACE

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/runtime.h> // for minimk_runtime_socket_t

#include <stddef.h> // for size_t
#include <stdint.h> // for UINT64_MAX
#include <string.h> // for memset

/// Maximum number of sockets managed by the runtime.
#define MAX_SOCKETS __MAX_HANDLES

/// Socket slot in the runtime table.
struct socketinfo {
    /// The actual handle associated to this entry.
    minimk_runtime_socket_t handle;

    /// The underlying OS socket file descriptor.
    minimk_socket_t fd;

    /// Read timeout in nanoseconds.
    uint64_t read_timeout;

    /// Write timeout in nanoseconds.
    uint64_t write_timeout;
};

/// Global socket table managed by the runtime.
static socketinfo sockets[MAX_SOCKETS];

/// Global generation counter - incremented each time we wrap around the table.
static uint64_t generation = 0;

/// Next slot to try allocating from.
static size_t next_slot = 0;

/// Find and validate socket info from ID, returning nullptr if invalid.
static minimk_error_t find_socketinfo(socketinfo **pinfo, minimk_runtime_socket_t handle) noexcept {
    // Zero the return argument
    *pinfo = nullptr;

    MINIMK_TRACE("trace: find_socketinfo handle=0x%llx\n", (unsigned long long)handle);

    // Reject handles owned by other subsystems
    uint8_t handle_type = __handle_type(handle);
    if (handle_type != __HANDLE_TYPE_SOCKET) {
        MINIMK_TRACE("trace: invalid handle type=%u, expected=%u\n", handle_type, __HANDLE_TYPE_SOCKET);
        return MINIMK_EBADF;
    }

    // Access the corresponding slot
    uint64_t index = __handle_index(handle);
    socketinfo *info = &sockets[index];

    MINIMK_TRACE("trace: checking slot %llu, stored_handle=0x%llx\n",
                 (unsigned long long)index,
                 (unsigned long long)info->handle);

    // Ensure the handle is actually correct
    if (info->handle != handle) {
        MINIMK_TRACE("trace: handle mismatch: expected=0x%llx, found=0x%llx\n",
                     (unsigned long long)handle,
                     (unsigned long long)info->handle);
        return MINIMK_EBADF;
    }

    *pinfo = info;
    return 0;
}

/// Create socketinfo for an existing socket fd.
static minimk_error_t create_socketinfo(socketinfo **pinfo, minimk_socket_t fd) noexcept {
    // Zero the return argument
    *pinfo = nullptr;

    MINIMK_TRACE("trace: create_socketinfo fd=%llu, next_slot=%zu, generation=%llu\n",
                 (unsigned long long)fd,
                 next_slot,
                 (unsigned long long)generation);

    // We need to search at most MAX_SOCKETS times before giving up
    for (size_t idx = 0; idx < MAX_SOCKETS && *pinfo == nullptr; idx++) {
        // Use modulo to stay within the bounds
        size_t slot_index = next_slot % MAX_SOCKETS;
        socketinfo *info = &sockets[slot_index];

        MINIMK_TRACE("trace: checking slot %zu (next_slot=%zu), handle=0x%llx\n",
                     slot_index,
                     next_slot,
                     (unsigned long long)info->handle);

        // A completely zero handle indicates that the slot is actually free
        if (info->handle == 0) {
            *pinfo = info;
            info->handle = __make_handle(__HANDLE_TYPE_SOCKET, generation, slot_index);
            info->fd = fd;
            info->read_timeout = UINT64_MAX;
            info->write_timeout = UINT64_MAX;

            MINIMK_TRACE("trace: allocated slot %zu, handle=0x%llx, fd=%llu\n",
                         slot_index,
                         (unsigned long long)info->handle,
                         (unsigned long long)fd);
            // FALLTHROUGH
        }

        // Increment the next slot to search
        next_slot++;

        // Check for a jump to the next generation
        if (next_slot >= 2 && (next_slot % MAX_SOCKETS) == 0) {
            generation++;
            MINIMK_TRACE("trace: generation incremented to %llu\n", (unsigned long long)generation);
        }
    }

    // Return the appropriate result
    if (*pinfo == nullptr) {
        MINIMK_TRACE("trace: no free slots available (EMFILE)\n");
        return MINIMK_EMFILE;
    }
    return 0;
}

/// Destroy socketinfo and free the slot.
static void destroy_socketinfo(socketinfo *info) noexcept {
    memset(info, 0, sizeof(*info));
}

minimk_error_t minimk_runtime_socket_create(minimk_runtime_socket_t *sock, //
                                            int domain,
                                            int type,
                                            int protocol) noexcept {
    // Invalidate the handle, as documented
    *sock = MINIMK_RUNTIME_INVALID_HANDLE;

    MINIMK_TRACE("trace: minimk_runtime_socket_create domain=%d type=%d protocol=%d\n", domain, type, protocol);

    // Create the underlying socket
    minimk_socket_t sockfd = minimk_socket_invalid();
    minimk_error_t rv = minimk_socket_create(&sockfd, domain, type, protocol);
    if (rv != 0) {
        MINIMK_TRACE("trace: minimk_socket_create failed: %d\n", rv);
        return rv;
    }

    MINIMK_TRACE("trace: created underlying socket fd=%llu\n", (unsigned long long)sockfd);

    // Make it nonblocking
    rv = minimk_socket_setnonblock(sockfd);
    if (rv != 0) {
        MINIMK_TRACE("trace: minimk_socket_setnonblock failed: %d\n", rv);
        minimk_socket_destroy(&sockfd);
        return rv;
    }

    // Create the corresponding socketinfo
    socketinfo *info = nullptr;
    rv = create_socketinfo(&info, sockfd);
    if (rv != 0) {
        MINIMK_TRACE("trace: create_socketinfo failed: %d\n", rv);
        minimk_socket_destroy(&sockfd);
        return rv;
    }

    // Return the socket handle to the caller
    *sock = info->handle;
    MINIMK_TRACE("trace: minimk_runtime_socket_create success: handle=0x%llx\n", (unsigned long long)*sock);
    return 0;
}

minimk_error_t
minimk_runtime_socket_bind(minimk_runtime_socket_t sock, const char *address, const char *port) noexcept {
    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, sock);
    return (rv != 0) ? rv : minimk_socket_bind(info->fd, address, port);
}

minimk_error_t minimk_runtime_socket_listen(minimk_runtime_socket_t sock, int backlog) noexcept {
    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, sock);
    return (rv != 0) ? rv : minimk_socket_listen(info->fd, backlog);
}

minimk_error_t minimk_runtime_socket_accept(minimk_runtime_socket_t *client_sock,
                                            minimk_runtime_socket_t listener_sock) noexcept {
    // Invalidate the handle, as documented
    *client_sock = MINIMK_RUNTIME_INVALID_HANDLE;

    // Find the corresponding info
    socketinfo *listener_info = nullptr;
    minimk_error_t rv = find_socketinfo(&listener_info, listener_sock);
    if (rv != 0) {
        return rv;
    }

    for (;;) {
        // Attempt to accept a connection
        minimk_socket_t client_fd = minimk_socket_invalid();
        minimk_error_t rv = minimk_socket_accept(&client_fd, listener_info->fd);

        // Suspend if needed
        if (rv == MINIMK_EAGAIN) {
            rv = minimk_runtime_suspend_read(listener_info->fd, listener_info->read_timeout);
            if (rv != 0) {
                return rv;
            }
            continue;
        }

        // Handle any other kind of errors
        if (rv != 0) {
            return rv;
        }

        // Make socket nonblocking
        rv = minimk_socket_setnonblock(client_fd);
        if (rv != 0) {
            minimk_socket_destroy(&client_fd);
            return rv;
        }

        // Create the corresponding socketinfo
        socketinfo *client_info = nullptr;
        rv = create_socketinfo(&client_info, client_fd);
        if (rv != 0) {
            minimk_socket_destroy(&client_fd);
            return rv;
        }

        // Return the socket handle to the caller
        *client_sock = client_info->handle;
        return 0;
    }
}

minimk_error_t
minimk_runtime_socket_recv(minimk_runtime_socket_t sock, void *data, size_t count, size_t *nread) noexcept {
    // Find the corresponding info
    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE("trace: minimk_runtime_socket_recv find_socketinfo failed: %d\n", rv);
        return rv;
    }

    MINIMK_TRACE("trace: minimk_runtime_socket_recv handle=0x%llx fd=%llu count=%zu\n", 
                 (unsigned long long)sock, (unsigned long long)info->fd, count);

    for (;;) {
        // Attempt to read data
        *nread = 0;
        minimk_error_t rv = minimk_socket_recv(info->fd, data, count, nread);

        MINIMK_TRACE("trace: minimk_socket_recv returned: rv=%d nread=%zu\n", rv, *nread);

        // We only need to continue trying on EAGAIN
        if (rv != MINIMK_EAGAIN) {
            return rv;
        }

        // Block until reading would not block
        rv = minimk_runtime_suspend_read(info->fd, info->read_timeout);
        if (rv != 0) {
            return rv;
        }
    }
}

minimk_error_t
minimk_runtime_socket_send(minimk_runtime_socket_t sock, const void *data, size_t count, size_t *nwritten) noexcept {
    // Find the corresponding info
    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, sock);
    if (rv != 0) {
        MINIMK_TRACE("trace: minimk_runtime_socket_send find_socketinfo failed: %d\n", rv);
        return rv;
    }

    MINIMK_TRACE("trace: minimk_runtime_socket_send handle=0x%llx fd=%llu count=%zu\n", 
                 (unsigned long long)sock, (unsigned long long)info->fd, count);

    for (;;) {
        // Attempt to send data
        *nwritten = 0;
        minimk_error_t rv = minimk_socket_send(info->fd, data, count, nwritten);

        MINIMK_TRACE("trace: minimk_socket_send returned: rv=%d nwritten=%zu\n", rv, *nwritten);

        // We only need to continue trying on EAGAIN
        if (rv != MINIMK_EAGAIN) {
            return rv;
        }

        // Block until ready
        rv = minimk_runtime_suspend_write(info->fd, info->write_timeout);
        if (rv != 0) {
            return rv;
        }
    }
}

minimk_error_t minimk_runtime_socket_destroy(minimk_runtime_socket_t *sock) noexcept {
    // Idempotent destroy
    if (*sock == MINIMK_RUNTIME_INVALID_HANDLE) {
        return 0;
    }

    // Find the corresponding info
    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, *sock);
    if (rv != 0) {
        *sock = MINIMK_RUNTIME_INVALID_HANDLE;
        return rv;
    }

    // Close the OS socket
    rv = minimk_socket_destroy(&info->fd);

    // Clear the slot
    destroy_socketinfo(info);

    // Invalidate the caller's handle
    *sock = MINIMK_RUNTIME_INVALID_HANDLE;

    // Return the closing error
    return rv;
}

minimk_error_t minimk_runtime_socket_sendall(minimk_runtime_socket_t sock, const void *buf, size_t count) noexcept {
    return __minimk_io_writeall<minimk_runtime_socket_t, minimk_runtime_socket_send>(sock, buf, count);
}

minimk_error_t minimk_runtime_socket_set_read_timeout(minimk_runtime_socket_t sock, uint64_t nanosec) noexcept {
    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, sock);
    if (rv != 0) {
        return rv;
    }

    info->read_timeout = nanosec;
    return 0;
}

minimk_error_t minimk_runtime_socket_set_write_timeout(minimk_runtime_socket_t sock, uint64_t nanosec) noexcept {
    socketinfo *info = nullptr;
    minimk_error_t rv = find_socketinfo(&info, sock);
    if (rv != 0) {
        return rv;
    }

    info->write_timeout = nanosec;
    return 0;
}

minimk_error_t minimk_runtime_socket_recvall(minimk_runtime_socket_t sock, void *buf, size_t count) noexcept {
    return __minimk_io_readall<minimk_runtime_socket_t, minimk_runtime_socket_recv>(sock, buf, count);
}
