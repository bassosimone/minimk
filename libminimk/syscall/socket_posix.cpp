// File: libminimk/syscall/socket_posix.cpp
// Purpose: socket library implemented for linux
// SPDX-License-Identifier: GPL-3.0-or-later

#include "socket_posix.hpp" // for minimk_syscall_socket__
#include "socket.h"         // for minimk_socket_t

#include <minimk/errno.h> // for minimk_error_t

minimk_error_t minimk_syscall_socket(minimk_syscall_socket_t *sock, int domain, int type,
                                     int protocol) noexcept {
    return minimk_syscall_socket__(sock, domain, type, protocol);
}
