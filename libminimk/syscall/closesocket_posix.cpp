// File: libminimk/syscall/closesocket_posix.cpp
// Purpose: close(2) implemented for POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "closesocket_posix.hpp" // for minimk_syscall_closesocket_impl

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_closesocket

minimk_error_t minimk_syscall_closesocket(minimk_syscall_socket_t *sock) noexcept {
    return minimk_syscall_closesocket_impl(sock);
}
