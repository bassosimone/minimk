// File: libminimk/syscall/accept_posix.cpp
// Purpose: accept(2) implemented for POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "accept_posix.hpp" // for minimk_syscall_accept_impl
#include "accept.h"         // for minimk_syscall_accept

#include <minimk/errno.h> // for minimk_error_t

minimk_error_t minimk_syscall_accept(minimk_syscall_socket_t *client_sock,
                                     minimk_syscall_socket_t sock) noexcept {
    return minimk_syscall_accept_impl(client_sock, sock);
}
