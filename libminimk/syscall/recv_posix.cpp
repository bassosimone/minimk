// File: libminimk/syscall/recv_posix.cpp
// Purpose: recv(2) implemented for POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "recv_posix.hpp" // for minimk_syscall_recv_impl

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_recv

#include <stddef.h> // for size_t

minimk_error_t minimk_syscall_recv(minimk_syscall_socket_t sock, void *data, size_t count,
                                   size_t *nread) noexcept {
    return minimk_syscall_recv_impl(sock, data, count, nread);
}
