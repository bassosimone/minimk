// File: include/minimk/socket.h
// Purpose: socket library
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_SOCKET_H
#define MINIMK_SOCKET_H

#include <minimk/core.h>  // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

#include <stddef.h> // for size_t
#include <stdint.h> // for uintptr_t

/// Type representing a socket descriptor.
///
/// The type is uintptr_t on Windows and int on POSIX.
#ifdef _WIN32
typedef uintptr_t minimk_socket_t;
#else
typedef int minimk_socket_t;
#endif

MINIMK_BEGIN_DECLS

/// Function for initializing the platform-specific socket library.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_init(void) MINIMK_NOEXCEPT;

/// Function returning the AF_INET value used by the platform.
///
/// The return value is AF_INET.
int minimk_socket_af_inet(void) MINIMK_NOEXCEPT;

/// Function returning the AF_INET6 value used by the platform.
///
/// The return value is AF_INET6.
int minimk_socket_af_inet6(void) MINIMK_NOEXCEPT;

/// Function returning the SOCK_STREAM value used by the platform.
///
/// The return value is SOCK_STREAM.
int minimk_socket_sock_stream(void) MINIMK_NOEXCEPT;

/// Function returning the SOCK_DGRAM value used by the platform.
///
/// The return value is SOCK_DGRAM.
int minimk_socket_sock_dgram(void) MINIMK_NOEXCEPT;

/// Function returning the platform-specific canonical invalid socket value.
///
/// The return value is -1 on Unix and INVALID_SOCKET on Windows.
minimk_socket_t minimk_socket_invalid(void) MINIMK_NOEXCEPT;

/// Function to create a new socket instance.
///
/// The sock return argument will be set to the invalid socket when the function
/// is invoked and later changed to a valid socket on success.
///
/// The domain MUST be AF_INET or AF_INET6.
///
/// The type MUST be SOCK_STREAM or SOCK_DGRAM.
///
/// The protocol must be zero.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_create(minimk_socket_t *sock, int domain, int type, int protocol) MINIMK_NOEXCEPT;

/// Function to configure a socket as nonblocking.
///
/// The sock argument must be a valid socket created using minimk_socket_create.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_setnonblock(minimk_socket_t sock) MINIMK_NOEXCEPT;

/// Function to establish a connection with a remote endpoint.
///
/// The sock argument must be a valid socket created using minimk_socket_create.
///
/// The address argument must be a valid IPv4 or IPv6 address represented as a
/// string.
///
/// The port argument must be a valid port represented as a string.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_connect(minimk_socket_t sock, const char *address, const char *port) MINIMK_NOEXCEPT;

/// Function to read bytes from a given socket.
///
/// The sock argument must be a valid socket created using minimk_socket_create.
///
/// The data argument must be a valid array containing the data.
///
/// The count argument must be the size of the array.
///
/// The nread argument is set to zero when the function is called and updated
/// to be the number of bytes read on success.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_recv(minimk_socket_t sock, void *data, size_t count, size_t *nread) MINIMK_NOEXCEPT;

/// Function to write bytes to a given socket.
///
/// The sock argument must be a valid socket created using minimk_socket_create.
///
/// The data argument must be a valid array containing the data.
///
/// The count argument must be the size of the array.
///
/// The nwritten argument is set to zero when the function is called and updated
/// to be the number of bytes written on success.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t
minimk_socket_send(minimk_socket_t sock, const void *data, size_t count, size_t *nwritten) MINIMK_NOEXCEPT;

/// Function to destroy a socket instance.
///
/// The sock argument must be a valid socket created using minimk_socket_create.
/// On return, the sock argument is unconditionally set to be an invalid socket.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_destroy(minimk_socket_t *sock) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // MINIMK_SOCKET_H
