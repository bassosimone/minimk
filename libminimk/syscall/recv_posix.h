// File: libminimk/syscall/recv_posix.h
// Purpose: recv(2) portable wrapper
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_RECV_POSIX_H
#define LIBMINIMK_SYSCALL_RECV_POSIX_H

#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

#include <stddef.h> // for size_t

MINIMK_BEGIN_DECLS

/// Function to read bytes from a given socket.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket.
///
/// The data argument must be a valid array for receiving the data.
///
/// The count argument specifies the maximum number of bytes to read.
///
/// The nread return argument will be set to the number of bytes actually read.
///
/// The return value is zero on success, MINIMK_EOF on end-of-file, or a
/// nonzero error code on failure.
minimk_error_t minimk_syscall_recv(minimk_syscall_socket_t sock, void *data, size_t count,
                                   size_t *nread) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_SYSCALL_RECV_POSIX_H
