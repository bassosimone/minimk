// File: libminimk/syscall/setsockopt_reuseaddr_posix.cpp
// Purpose: setsockopt(2) implemented for POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "setsockopt_reuseaddr_posix.hpp" // for minimk_syscall_setsockopt_reuseaddr_impl

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_setsockopt_reuseaddr

MINIMK_BEGIN_DECLS

minimk_error_t minimk_syscall_setsockopt_reuseaddr(minimk_syscall_socket_t sock) MINIMK_NOEXCEPT {
    return minimk_syscall_setsockopt_reuseaddr_impl(sock);
}

MINIMK_END_DECLS
