// File: libminimk/socket/sendall.cpp
// Purpose: sendall implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sendall.hpp" // for minimk_socket_sendall_impl

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_t

#include <stddef.h> // for size_t

minimk_error_t minimk_socket_sendall(minimk_socket_t sock, const void *buf, size_t count) noexcept {
    return minimk_socket_sendall_impl(sock, buf, count);
}
