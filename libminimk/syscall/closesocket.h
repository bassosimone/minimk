// File: libminimk/syscall/closesocket_posix.h
// Purpose: close(2) portable wrapper
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_CLOSESOCKET_POSIX_H
#define LIBMINIMK_SYSCALL_CLOSESOCKET_POSIX_H

#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

MINIMK_BEGIN_DECLS

/// Function to close a socket instance.
///
/// This function is thread-safe.
///
/// The sock parameter will be set to the invalid socket after closing.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_syscall_closesocket(minimk_syscall_socket_t *sock) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_SYSCALL_CLOSESOCKET_POSIX_H
