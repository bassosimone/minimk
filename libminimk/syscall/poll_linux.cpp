// File: libminimk/syscall/poll_linux.cpp
// Purpose: linux poll(2) wrapper
// SPDX-License-Identifier: GPL-3.0-or-later

#include "poll_linux.hpp" // for minimk_syscall_poll__
#include "poll.h"         // for minimk_syscall_poll

#include <minimk/errno.h> // for minimk_error_t

short minimk_syscall_pollin(void) noexcept {
    return POLLIN;
}

short minimk_syscall_pollout(void) noexcept {
    return POLLOUT;
}

short minimk_syscall_pollerr(void) noexcept {
    return POLLERR;
}

minimk_error_t minimk_syscall_poll(struct minimk_syscall_pollfd *fds, size_t size, int timeout,
                                   size_t *nready) noexcept {
    return minimk_syscall_poll__(fds, size, timeout, nready);
}
