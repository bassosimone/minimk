// File: libminimk/socket/listen.cpp
// Purpose: listen implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "listen.hpp" // for minimk_socket_listen_impl

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_t

minimk_error_t minimk_socket_listen(minimk_socket_t sock, int backlog) noexcept {
    return minimk_socket_listen_impl(sock, backlog);
}
