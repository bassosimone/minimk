// File: include/minimk/poll.h
// Purpose: poll library
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_POLL_H
#define MINIMK_POLL_H

#include <minimk/core.h>   // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_t

#include <stddef.h> // for size_t

/// Type representing a pollable descriptor.
struct minimk_pollfd {
    minimk_socket_t fd;
    short events;
    short revents;
};

MINIMK_BEGIN_DECLS

/// Function returning the POLLIN value used by the platform.
short minimk_poll_pollin(void) MINIMK_NOEXCEPT;

/// Function returning the POLLOUT value used by the platform.
short minimk_poll_pollout(void) MINIMK_NOEXCEPT;

/// Function returning the POLLERR value used by the platform.
short minimk_poll_pollerr(void) MINIMK_NOEXCEPT;

/// Function that blocks until sockets become readable/writable, a timeout expires
/// or we are interrupted by a signal (thus returning MINIMK_EINTR).
///
/// The fds argument points to an array of fds.
///
/// The size is the size of the array (we allow up to UINT16_MAX fds currently).
///
/// The timeout is the number of milliseconds to wait, if positive, zero to
/// avoid blocking, and negative to block until I/O or signal.
///
/// The nready return argument contains the number of ready descriptors.
///
/// The return value is zero on success or a nonzero error code on failure. Note
/// that success includes the case where no descriptors are ready.
minimk_error_t minimk_poll(struct minimk_pollfd *fds, size_t size, int timeout, size_t *nready) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // MINIMK_POLL_H
