// File: libminimk/poll/poll_linux.hpp
// Purpose: poll library implemented for linux
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_POLL_POLL_LINUX_HPP
#define LIBMINIMK_POLL_POLL_LINUX_HPP

#include "poll.h" // for minimk_poll

#include <minimk/errno.h> // for minimk_error_t

#include <poll.h>   // for poll
#include <stddef.h> // for offsetof
#include <stdint.h> // for UINT16_MAX

// Ensure the structure definition is binary compatible
static_assert(sizeof(struct minimk_pollfd) == sizeof(struct pollfd), "minimk_pollfd must have same size as pollfd");

static_assert(offsetof(struct minimk_pollfd, fd) == offsetof(struct pollfd, fd),
              "minimk_pollfd.fd offset must match pollfd.fd");

static_assert(offsetof(struct minimk_pollfd, events) == offsetof(struct pollfd, events),
              "minimk_pollfd.events offset must match pollfd.events");

static_assert(offsetof(struct minimk_pollfd, revents) == offsetof(struct pollfd, revents),
              "minimk_pollfd.revents offset must match pollfd.revents");

// Testable minimk_poll implementation.
template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(minimk_errno_get) __minimk_errno_get = minimk_errno_get,
          decltype(poll) __sys_poll = poll>
minimk_error_t __minimk_poll(struct minimk_pollfd *fds, size_t size, int timeout, size_t *nready) noexcept {
    size = (size <= UINT16_MAX) ? size : UINT16_MAX;
    __minimk_errno_clear();
    int rv = __sys_poll((struct pollfd *)fds, (nfds_t)size, timeout);
    int success = (rv >= 0);
    *nready = (success) ? (size_t)rv : 0;
    return (success) ? 0 : __minimk_errno_get();
}

#endif // LIBMINIMK_POLL_POLL_LINUX_HPP
