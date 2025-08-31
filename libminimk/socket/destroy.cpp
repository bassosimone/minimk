// File: libminimk/socket/destroy.cpp
// Purpose: destroy implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "destroy.hpp" // for minimk_socket_destroy_impl

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_t

minimk_error_t minimk_socket_destroy(minimk_socket_t *sock) noexcept {
    return minimk_socket_destroy_impl(sock);
}
