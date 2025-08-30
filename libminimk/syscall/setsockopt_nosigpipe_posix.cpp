// File: libminimk/syscall/setsockopt_nosigpipe_posix.cpp
// Purpose: setsockopt(2) implemented for POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "setsockopt_nosigpipe_posix.hpp" // for minimk_syscall_setsockopt_nosigpipe__
#include "setsockopt_nosigpipe.h"         // for minimk_syscall_setsockopt_nosigpipe

#include <minimk/errno.h> // for minimk_error_t

minimk_error_t minimk_syscall_setsockopt_nosigpipe(minimk_syscall_socket_t sock) noexcept {
    return minimk_syscall_setsockopt_nosigpipe__(sock);
}
