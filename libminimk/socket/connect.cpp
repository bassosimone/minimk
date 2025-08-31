// File: libminimk/socket/connect.cpp
// Purpose: connect implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "connect.hpp" // for minimk_socket_connect_impl

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_t

minimk_error_t minimk_socket_connect(minimk_socket_t sock, const char *address, const char *port) noexcept {
    return minimk_socket_connect_impl(sock, address, port);
}
