// File: libminimk/socket/send.cpp
// Purpose: send implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "send.hpp" // for minimk_socket_send_impl

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_t

#include <stddef.h> // for size_t

minimk_error_t minimk_socket_send(minimk_socket_t sock, const void *data, size_t count,
                                  size_t *nwritten) noexcept {
    return minimk_socket_send_impl(sock, data, count, nwritten);
}
