// File: libminimk/socket/accept.cpp
// Purpose: implements accept
// SPDX-License-Identifier: GPL-3.0-or-later

#include "accept.hpp" // for minimk_socket_accept_impl

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_t

minimk_error_t minimk_socket_accept(minimk_socket_t *client_sock, minimk_socket_t listener_sock) noexcept {
    return minimk_socket_accept_impl(client_sock, listener_sock);
}
