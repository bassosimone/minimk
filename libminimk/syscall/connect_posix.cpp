// File: libminimk/syscall/connect_posix.cpp
// Purpose: connect(2) implemented for POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "connect_posix.hpp" // for minimk_syscall_connect_impl
#include "connect.h"         // for minimk_syscall_connect

#include <minimk/errno.h> // for minimk_error_t

minimk_error_t minimk_syscall_connect(minimk_syscall_socket_t sock, const char *address,
                                      const char *port) noexcept {
    return minimk_syscall_connect_impl(sock, address, port);
}
