// File: libminimk/syscall/poll.h
// Purpose: portable poll(2) wrapper
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_POLL_H
#define LIBMINIMK_SYSCALL_POLL_H

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

#include <stddef.h> // for size_t

#ifdef _WIN32
#include <winsock2.h>
#else
#include <poll.h>
#endif

/// Type representing a pollable descriptor.
#ifdef _WIN32
typedef WSAPOLLFD minimk_syscall_pollfd_t;
#else
typedef struct pollfd minimk_syscall_pollfd_t;
#endif

MINIMK_BEGIN_DECLS

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
minimk_error_t minimk_syscall_poll(minimk_syscall_pollfd_t *fds, size_t size, int timeout,
                                   size_t *nready) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

/// Variable containing the platform's POLLIN definition.
extern short minimk_syscall_pollin;

/// Variable containing the platform's POLLOUT definition.
extern short minimk_syscall_pollout;

/// Variable containing the platform's POLLERR definition.
extern short minimk_syscall_pollerr;

#endif // LIBMINIMK_SYSCALL_POLL_H
