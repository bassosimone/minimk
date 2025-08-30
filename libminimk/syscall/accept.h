// File: libminimk/syscall/accept_posix.h
// Purpose: accept(2) portable wrapper
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_ACCEPT_POSIX_H
#define LIBMINIMK_SYSCALL_ACCEPT_POSIX_H

#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

MINIMK_BEGIN_DECLS

/// Function to accept a connection on a listening socket.
///
/// This function is thread-safe.
///
/// The client_sock return argument will be set to the invalid socket when the function
/// is invoked and later changed to a valid socket on success.
///
/// The sock argument must be a valid listening socket.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_syscall_accept(minimk_syscall_socket_t *client_sock,
                                     minimk_syscall_socket_t sock) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_SYSCALL_ACCEPT_POSIX_H
