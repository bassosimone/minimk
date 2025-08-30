// File: libminimk/socket/socket.h
// Purpose: private socket operations (low-level blocking API)
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SOCKET_SOCKET_H
#define LIBMINIMK_SOCKET_SOCKET_H

#include "../syscall/socket.h"

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

#include <stddef.h> // for size_t

/// Type representing a socket descriptor.
///
/// The type is uintptr_t on Windows and int on POSIX.
#define minimk_socket_t minimk_syscall_socket_t

MINIMK_BEGIN_DECLS

/// Function to configure a socket as nonblocking.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket created using minimk_socket_create.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_setnonblock(minimk_socket_t sock) MINIMK_NOEXCEPT;

/// Function to read bytes from a given socket.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket created using minimk_socket_create.
///
/// The data argument must be a valid array for receiving the data.
///
/// The count argument must be the size of the array.
///
/// The nread argument is set to zero when the function is called and updated
/// to be the number of bytes read on success.
///
/// This function increases robustness as follows:
///
/// 1. if count is zero, it returns MINIMK_EINVAL since reading zero bytes is most
/// likely a bug in the code and should not actually happen.
///
/// 2. otherwise, if recv returns zero, it returns MINIMK_EOF.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_recv(minimk_socket_t sock, void *data, size_t count,
                                  size_t *nread) MINIMK_NOEXCEPT;

/// Function to write bytes to a given socket.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket created using minimk_socket_create.
///
/// The data argument must be a valid array containing the data to send.
///
/// The count argument must be the size of the array.
///
/// The nwritten argument is set to zero when the function is called and updated
/// to be the number of bytes written on success.
///
/// This function increases robustness as follows:
///
/// 1. if count is zero, it returns MINIMK_EINVAL since writing zero bytes is most
/// likely a bug in the code and should not actually happen.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_send(minimk_socket_t sock, const void *data, size_t count,
                                  size_t *nwritten) MINIMK_NOEXCEPT;

/// Like minimk_socket_recv but receives all requested bytes unless an error occurs.
///
/// In case of short read, returns the error that occurred. This is an all-or-nothing operation.
///
/// When interruped by a signal, this function continues to read relentlessly.
minimk_error_t minimk_socket_recvall(minimk_socket_t sock, void *buf, size_t count) MINIMK_NOEXCEPT;

/// Like minimk_socket_send but sends all the content of the buffer unless an error occurs.
///
/// In case of short write, returns the error that occurred. This is an all-or-nothing operation.
///
/// When interruped by a signal, this function continues to write relentlessly.
minimk_error_t minimk_socket_sendall(minimk_socket_t sock, const void *buf,
                                     size_t count) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_SOCKET_SOCKET_H
