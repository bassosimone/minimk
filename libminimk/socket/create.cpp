// File: libminimk/socket/create.cpp
// Purpose: create implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "create.hpp" // for minimk_socket_create_impl

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_t

minimk_error_t minimk_socket_create(minimk_socket_t *sock, int domain, int type, int protocol) noexcept {
    return minimk_socket_create_impl(sock, domain, type, protocol);
}
