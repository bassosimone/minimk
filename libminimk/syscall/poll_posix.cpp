// File: libminimk/syscall/poll_posix.cpp
// Purpose: POSIX poll(2) wrapper
// SPDX-License-Identifier: GPL-3.0-or-later

#include "poll_posix.hpp" // for minimk_syscall_poll__
#include "poll.h"         // for minimk_syscall_poll

#include <minimk/errno.h> // for minimk_error_t

short minimk_syscall_pollin = POLLIN;
short minimk_syscall_pollout = POLLOUT;
short minimk_syscall_pollerr = POLLERR;

minimk_error_t minimk_syscall_poll(struct minimk_syscall_pollfd *fds, size_t size, int timeout,
                                   size_t *nready) noexcept {
    return minimk_syscall_poll__(fds, size, timeout, nready);
}
