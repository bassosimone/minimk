// File: libminimk/syscall/socket_setnonblock_posix.h
// Purpose: fcntl(2) portable wrapper for setting socket nonblocking
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SOCKET_SETNONBLOCK_POSIX_H
#define LIBMINIMK_SYSCALL_SOCKET_SETNONBLOCK_POSIX_H

#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

MINIMK_BEGIN_DECLS

/// Function to configure a socket as nonblocking.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_syscall_socket_setnonblock(minimk_syscall_socket_t sock) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_SYSCALL_SOCKET_SETNONBLOCK_POSIX_H
