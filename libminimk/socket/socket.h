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

/// Function to establish a connection with a remote endpoint.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket created using minimk_socket_create.
///
/// The address argument must be a valid IPv4 or IPv6 address in numeric format
/// represented as a string (e.g., "127.0.0.1", "::1").
///
/// The port argument must be a valid port number represented as a string (e.g., "80").
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_connect(minimk_socket_t sock, const char *address,
                                     const char *port) MINIMK_NOEXCEPT;

/// Function to bind a socket to a local address and port.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket created using minimk_socket_create.
///
/// The address argument must be a valid IPv4 or IPv6 address in numeric format
/// represented as a string (e.g., "127.0.0.1", "::1"). Use "0.0.0.0" for IPv4
/// or "::" for IPv6 to bind to all available interfaces.
///
/// The port argument must be a valid port number represented as a string (e.g., "8080").
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_bind(minimk_socket_t sock, const char *address,
                                  const char *port) MINIMK_NOEXCEPT;

/// Function to mark a socket as listening for incoming connections.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket created using minimk_socket_create
/// and bound using minimk_socket_bind.
///
/// The backlog argument specifies the maximum length of the queue of pending
/// connections. A value of 128 is typically reasonable for most applications.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_listen(minimk_socket_t sock, int backlog) MINIMK_NOEXCEPT;

/// Function to accept an incoming connection on a listening socket.
///
/// This function is thread-safe.
///
/// The client_sock return argument will be set to the invalid socket when the function
/// is invoked and later changed to a valid socket on success representing the
/// accepted client connection.
///
/// The sock argument must be a valid socket created using minimk_socket_create,
/// bound using minimk_socket_bind, and marked as listening using minimk_socket_listen.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_accept(minimk_socket_t *client_sock,
                                    minimk_socket_t sock) MINIMK_NOEXCEPT;

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
