// File: libminimk/socket/destroy.hpp
// Purpose: destroy implementation
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SOCKET_DESTROY_HPP
#define LIBMINIMK_SOCKET_DESTROY_HPP

#include "../cast/static.hpp" // for CAST_ULL

#include "info.hpp" // for struct socket_info

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/socket.h>  // for minimk_socket_t
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/trace.h>   // for MINIMK_TRACE_SOCKET

/// Testable minimk_socket_destroy implementation.
template <
        decltype(minimk_socket_info_find) M_info_find = minimk_socket_info_find,
        decltype(minimk_syscall_closesocket) M_closesocket = minimk_syscall_closesocket,
        decltype(minimk_socket_info_forget) M_info_forget = minimk_socket_info_forget>
minimk_error_t minimk_socket_destroy_impl(minimk_socket_t *sock) noexcept {
    MINIMK_TRACE_SOCKET("destroy handle=0x%llx\n", CAST_ULL(*sock));

    // Idempotent destroy
    if (*sock == MINIMK_SOCKET_INVALID) {
        MINIMK_TRACE_SOCKET("destroy result=%s\n", minimk_errno_name(0));
        return 0;
    }

    // Find the corresponding info
    socket_info *info = nullptr;
    minimk_error_t rv = M_info_find(&info, *sock);
    if (rv != 0) {
        *sock = MINIMK_SOCKET_INVALID;
        MINIMK_TRACE_SOCKET("destroy result=%s\n", minimk_errno_name(rv));
        return rv;
    }

    MINIMK_TRACE_SOCKET("destroy fd=%llu\n", CAST_ULL(info->fd));

    // Close the OS socket
    rv = M_closesocket(&info->fd);

    // Clear the slot
    M_info_forget(info);

    // Invalidate the caller's handle
    *sock = MINIMK_SOCKET_INVALID;

    // Return the closing error
    MINIMK_TRACE_SOCKET("destroy result=%s\n", minimk_errno_name(rv));
    return rv;
}

#endif // LIBMINIMK_SOCKET_DESTROY_HPP
