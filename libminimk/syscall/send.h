// File: libminimk/syscall/send_posix.h
// Purpose: send(2) portable wrapper
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SEND_POSIX_H
#define LIBMINIMK_SYSCALL_SEND_POSIX_H

#include "socket.h" // for minimk_syscall_socket_t

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

#include <stddef.h> // for size_t

MINIMK_BEGIN_DECLS

/// Function to write bytes to a given socket.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket.
///
/// The data argument must be a valid array containing the data to send.
///
/// The count argument specifies the number of bytes to write.
///
/// The nwritten return argument will be set to the number of bytes actually written.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_syscall_send(minimk_syscall_socket_t sock, const void *data, size_t count,
                                   size_t *nwritten) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_SYSCALL_SEND_POSIX_H
