// File: libminimk/syscall/getsockopt_error.h
// Purpose: getsockopt(2) portable wrapper for SO_ERROR
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_GETSOCKOPT_ERROR_H
#define LIBMINIMK_SYSCALL_GETSOCKOPT_ERROR_H

#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

MINIMK_BEGIN_DECLS

/// Function to get SO_ERROR socket option.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket.
///
/// The error output argument will be set to the socket error value mapped
/// to minimk_error_t. A value of 0 indicates no error (connection succeeded),
/// while nonzero values indicate the specific error that occurred.
///
/// This function is typically used after a nonblocking connect() returns
/// EINPROGRESS to determine the final connection status once the socket
/// becomes writeable.
///
/// The return value is zero on success (getsockopt succeeded) or a nonzero
/// error code on failure (getsockopt failed).
minimk_error_t minimk_syscall_getsockopt_error(minimk_syscall_socket_t sock,
                                               minimk_error_t *error) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_SYSCALL_GETSOCKOPT_ERROR_H
