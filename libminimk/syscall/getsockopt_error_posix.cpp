// File: libminimk/syscall/getsockopt_error_posix.cpp
// Purpose: getsockopt(2) implemented for POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "getsockopt_error_posix.hpp" // for minimk_syscall_getsockopt_error_impl
#include "getsockopt_error.h"         // for minimk_syscall_getsockopt_error

#include <minimk/errno.h> // for minimk_error_t

minimk_error_t minimk_syscall_getsockopt_error(minimk_syscall_socket_t sock,
                                               minimk_error_t *error) noexcept {
    return minimk_syscall_getsockopt_error_impl(sock, error);
}
