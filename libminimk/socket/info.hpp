// File: libminimk/socket/info.hpp
// Purpose: socket_info table management
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SOCKET_INFO_HPP
#define LIBMINIMK_SOCKET_INFO_HPP

#include "handle.hpp" // for MAX_HANDLES
#include "minimk/cdefs.h"

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/socket.h>  // for minimk_socket_t
#include <minimk/syscall.h> // for minimk_syscall_*

#include <stddef.h> // for size_t
#include <stdint.h> // for UINT64_MAX

/// Maximum number of sockets managed by the runtime.
#define MAX_SOCKETS MAX_HANDLES

/// Socket slot in the runtime table.
struct socket_info {
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

MINIMK_BEGIN_DECLS

/// Function for accessing a specific sockets table entry.
///
/// The return value cannot be NULL. However, the entry may belong
/// to a previous generation and could thus be invalid.
socket_info *minimk_socket_info_get(size_t idx) noexcept;

/// Function for getting an already existing entry given a handle.
///
/// The pinfo argument is where we store the entry. On failure, we set it to point to. NULL.
///
/// The handle is the socket handle for which to find an entry.
///
/// The return value is zero on success and a nonzero error code on failure.
minimk_error_t minimk_socket_info_find(socket_info **pinfo, minimk_socket_t handle) noexcept;

/// Function to create a new socketinfo for a specific syscall socket descriptor.
///
/// The pinfo argument is where we store the entry. On failure, we set it to point to NULL.
///
/// The fd argument is the syscall socket descriptor for which to create a new entry.
///
/// The return value is zero on success and a nonzero error code on failure.
minimk_error_t minimk_socket_info_create(socket_info **pinfo, minimk_syscall_socket_t fd) noexcept;

/// Function to clear a given entry and make it reusable in the future.
///
/// You must close the underlying socket before doing this.
void minimk_socket_info_forget(socket_info *info) noexcept;

MINIMK_END_DECLS

#endif // LIBMINIMK_SOCKET_INFO_HPP
