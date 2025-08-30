// File: libminimk/syscall/bind_posix.cpp
// Purpose: bind(2) implemented for POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bind_posix.h"   // for minimk_syscall_bind
#include "bind_posix.hpp" // for minimk_syscall_bind__

#include <minimk/errno.h> // for minimk_error_t

minimk_error_t minimk_syscall_bind(minimk_syscall_socket_t sock, const char *address,
                                   const char *port) noexcept {
    return minimk_syscall_bind__(sock, address, port);
}
