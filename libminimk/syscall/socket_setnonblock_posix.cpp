// File: libminimk/syscall/socket_setnonblock_posix.cpp
// Purpose: fcntl(2) implemented for POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "socket_setnonblock_posix.h"   // for minimk_syscall_socket_setnonblock
#include "socket_setnonblock_posix.hpp" // for minimk_syscall_socket_setnonblock__

#include <minimk/errno.h> // for minimk_error_t

minimk_error_t minimk_syscall_socket_setnonblock(minimk_syscall_socket_t sock) noexcept {
    return minimk_syscall_socket_setnonblock__(sock);
}
