// File: libminimk/syscall/listen_posix.cpp
// Purpose: listen(2) implemented for POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "listen.h"   // for minimk_syscall_listen
#include "listen_posix.hpp" // for minimk_syscall_listen__

#include <minimk/errno.h> // for minimk_error_t

minimk_error_t minimk_syscall_listen(minimk_syscall_socket_t sock, int backlog) noexcept {
    return minimk_syscall_listen__(sock, backlog);
}
