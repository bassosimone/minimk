// File: libminimk/socket/recvall.cpp
// Purpose: recvall implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "recvall.hpp" // for minimk_socket_recvall_impl

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_t

#include <stddef.h> // for size_t

minimk_error_t minimk_socket_recvall(minimk_socket_t sock, void *buf, size_t count) noexcept {
    return minimk_socket_recvall_impl(sock, buf, count);
}
