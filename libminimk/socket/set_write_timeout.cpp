// File: libminimk/socket/set_write_timeout.cpp
// Purpose: set_write_timeout implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "set_write_timeout.hpp" // for minimk_socket_set_write_timeout_impl

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_t

#include <stdint.h> // for uint64_t

minimk_error_t minimk_socket_set_write_timeout(minimk_socket_t sock, uint64_t nanosec) noexcept {
    return minimk_socket_set_write_timeout_impl(sock, nanosec);
}
