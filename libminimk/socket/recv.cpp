// File: libminimk/socket/recv.cpp
// Purpose: recv implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "recv.hpp" // for minimk_socket_recv_impl

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_t

#include <stddef.h> // for size_t

minimk_error_t minimk_socket_recv(minimk_socket_t sock, void *data, size_t count, size_t *nread) noexcept {
    return minimk_socket_recv_impl(sock, data, count, nread);
}
