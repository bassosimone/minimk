// File: libminimk/syscall/setsockopt_nosigpipe.h
// Purpose: setsockopt(2) portable wrapper for SO_NOSIGPIPE
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SETSOCKOPT_NOSIGPIPE_H
#define LIBMINIMK_SYSCALL_SETSOCKOPT_NOSIGPIPE_H

#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

MINIMK_BEGIN_DECLS

/// Function to set SO_NOSIGPIPE socket option.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket.
///
/// On platforms that support SO_NOSIGPIPE (BSD variants), this function
/// sets the socket option to avoid SIGPIPE when writing to a connection
/// closed by the peer. On other platforms, this function is a no-op.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_syscall_setsockopt_nosigpipe(minimk_syscall_socket_t sock) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_SYSCALL_SETSOCKOPT_NOSIGPIPE_H
