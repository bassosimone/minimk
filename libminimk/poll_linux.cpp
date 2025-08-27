// File: libminimk/poll_linux.cpp
// Purpose: poll library for linux
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/poll.h>

#include "poll_linux.hpp"

short minimk_poll_pollin(void) noexcept {
    return POLLIN;
}

short minimk_poll_pollout(void) noexcept {
    return POLLOUT;
}

short minimk_poll_pollerr(void) noexcept {
    return POLLERR;
}

minimk_error_t minimk_poll(struct minimk_pollfd *fds, size_t size, int timeout, size_t *nready) noexcept {
    return __minimk_poll(fds, size, timeout, nready);
}
