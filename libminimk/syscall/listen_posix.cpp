// File: libminimk/syscall/listen_posix.cpp
// Purpose: listen(2) implemented for POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "listen_posix.hpp" // for minimk_syscall_listen_impl

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_listen

minimk_error_t minimk_syscall_listen(minimk_syscall_socket_t sock, int backlog) noexcept {
    return minimk_syscall_listen_impl(sock, backlog);
}
