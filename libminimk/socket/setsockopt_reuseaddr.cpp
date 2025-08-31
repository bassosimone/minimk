// File: libminimk/socket/setsockopt_reuseaddr.cpp
// Purpose: setsockopt_reuseraddr implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "setsockopt_reuseaddr.hpp" // for minimk_socket_setsockopt_reuseaddr_impl

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_t

minimk_error_t minimk_socket_setsockopt_reuseaddr(minimk_socket_t sock) noexcept {
    return minimk_socket_setsockopt_reuseaddr_impl(sock);
}
