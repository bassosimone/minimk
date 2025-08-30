// File: libminimk/socket/socket.h
// Purpose: private socket operations (low-level blocking API)
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SOCKET_SOCKET_H
#define LIBMINIMK_SOCKET_SOCKET_H

#include <minimk/cdefs.h>   // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_*

#include <stddef.h> // for size_t

MINIMK_BEGIN_DECLS

/// Like send but sends all the content of the buffer unless an error occurs.
///
/// In case of short write, returns the error that occurred. This is an all-or-nothing operation.
///
/// When interruped by a signal, this function continues to write relentlessly.
minimk_error_t minimk_socket_sendall(minimk_syscall_socket_t sock, const void *buf,
                                     size_t count) MINIMK_NOEXCEPT;

/// Like recv but receives all requested bytes unless an error occurs.
///
/// In case of short read, returns the error that occurred. This is an all-or-nothing operation.
///
/// When interrupted by a signal, this function continues to read relentlessly.
minimk_error_t minimk_socket_recvall(minimk_syscall_socket_t sock, void *buf,
                                     size_t count) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_SOCKET_SOCKET_H
