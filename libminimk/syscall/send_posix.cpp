// File: libminimk/syscall/send_posix.cpp
// Purpose: send(2) implemented for POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "send_posix.hpp" // for minimk_syscall_send_impl

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_send

#include <stddef.h> // for size_t

minimk_error_t minimk_syscall_send(minimk_syscall_socket_t sock, const void *data, size_t count,
                                   size_t *nwritten) noexcept {
    return minimk_syscall_send_impl(sock, data, count, nwritten);
}
