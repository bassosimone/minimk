// File: libminimk/socket/info.cpp
// Purpose: socket_info table management
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../cast/static.hpp" // for CAST_ULL

#include "handle.hpp" // for make_handle
#include "info.hpp"   // for struct socket_info

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/cdefs.h>   // for MINIMK_UNSAFE_BUFFER_USAGE_*
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/socket.h>  // for minimk_socket_t
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/trace.h>   // for MINIMK_TRACE_SOCKET

#include <stddef.h> // for size_t
#include <stdint.h> // for UINT64_MAX

/// Global socket table managed by the runtime.
static socket_info sockets[MAX_SOCKETS];

/// Global generation counter - incremented each time we wrap around the table.
static uint64_t generation = 0;

/// Next slot to try allocating from.
static size_t next_slot = 0;

socket_info *minimk_socket_info_get(size_t idx) noexcept {
    MINIMK_ASSERT(idx >= 0 && idx < MAX_SOCKETS);
    MINIMK_UNSAFE_BUFFER_USAGE_BEGIN
    return &sockets[idx];
    MINIMK_UNSAFE_BUFFER_USAGE_END
}

minimk_error_t minimk_socket_info_find(socket_info **pinfo, minimk_socket_t handle) noexcept {
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
    socket_info *info = minimk_socket_info_get(index);
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

minimk_error_t minimk_socket_info_create(socket_info **pinfo, minimk_syscall_socket_t fd) noexcept {
    // Zero the return argument
    *pinfo = nullptr;

    MINIMK_TRACE_SOCKET("create_socketinfo next_slot=%zu\n", next_slot);
    MINIMK_TRACE_SOCKET("create_socketinfo generation=%llu\n", CAST_ULL(generation));

    // We need to search at most MAX_SOCKETS times before giving up
    for (size_t idx = 0; idx < MAX_SOCKETS && *pinfo == nullptr; idx++) {
        // Use modulo to stay within the bounds
        size_t slot_index = next_slot % MAX_SOCKETS;
        socket_info *info = minimk_socket_info_get(slot_index);

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

void minimk_socket_info_forget(socket_info *info) noexcept {
    *info = {};
}
