// File: libminimk/syscall/connect_posix.h
// Purpose: connect(2) portable wrapper
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_CONNECT_POSIX_H
#define LIBMINIMK_SYSCALL_CONNECT_POSIX_H

#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

MINIMK_BEGIN_DECLS

/// Function to establish a connection with a remote endpoint.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket.
///
/// The address MUST be an IPv4 or IPv6 address in string form.
///
/// The port MUST be a port number in string form.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_syscall_connect(minimk_syscall_socket_t sock, const char *address,
                                      const char *port) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_SYSCALL_CONNECT_POSIX_H
