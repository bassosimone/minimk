// File: libminimk/syscall/poll.h
// Purpose: portable poll(2) wrapper
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_POLL_H
#define LIBMINIMK_SYSCALL_POLL_H

#include "../socket/socket.h" // for minimk_socket_t

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

#include <stddef.h> // for size_t

/// Type representing a pollable descriptor.
///
/// Each port should ensure that this type is binary
/// compatible with the platform's `struct poll`.
struct minimk_syscall_pollfd {
    minimk_socket_t fd;
    short events;
    short revents;
};

MINIMK_BEGIN_DECLS

/// Function returning the POLLIN value used by the platform.
///
/// This function is thread-safe.
short minimk_syscall_pollin(void) MINIMK_NOEXCEPT;

/// Function returning the POLLOUT value used by the platform.
///
/// This function is thread-safe.
short minimk_syscall_pollout(void) MINIMK_NOEXCEPT;

/// Function returning the POLLERR value used by the platform.
///
/// This function is thread-safe.
short minimk_syscall_pollerr(void) MINIMK_NOEXCEPT;

/// Function that blocks until sockets become readable/writable, a timeout expires
/// or we are interrupted by a signal (thus returning MINIMK_EINTR).
///
/// This function is thread-safe.
///
/// The fds argument points to an array of fds.
///
/// The size is the size of the array (we allow up to UINT16_MAX fds currently).
///
/// The timeout is the number of milliseconds to wait, if positive, zero to
/// avoid blocking, and negative to block until I/O or signal.
///
/// The nready return argument is set to zero when the function is called and
/// contains the number of ready descriptors on success.
///
/// The return value is zero on success or a nonzero error code on failure. Note
/// that success includes the case where no descriptors are ready.
minimk_error_t minimk_syscall_poll(struct minimk_syscall_pollfd *fds, size_t size, int timeout,
                                   size_t *nready) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_SYSCALL_POLL_H
