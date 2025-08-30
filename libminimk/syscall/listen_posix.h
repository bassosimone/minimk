// File: libminimk/syscall/listen_posix.h
// Purpose: listen(2) portable wrapper
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_LISTEN_POSIX_H
#define LIBMINIMK_SYSCALL_LISTEN_POSIX_H

#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

MINIMK_BEGIN_DECLS

/// Function to mark a socket as passive, ready to accept connections.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket.
///
/// The backlog argument specifies the maximum number of pending connections.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_syscall_listen(minimk_syscall_socket_t sock, int backlog) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_SYSCALL_LISTEN_POSIX_H
