// File: include/minimk/syscall.h
// Purpose: portable system call interface
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_SYSCALL_H
#define MINIMK_SYSCALL_H

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

#include <stddef.h> // for size_t

#ifdef _WIN32
#include <winsock2.h>
#else
#include <poll.h>
#endif

/// Type representing a descriptor mananged by poll.
#ifdef _WIN32
typedef WSAPOLLFD minimk_syscall_pollfd_t;
#else
typedef struct pollfd minimk_syscall_pollfd_t;
#endif

/// Type representing a socket descriptor.
#ifdef _WIN32
typedef SOCKET minimk_syscall_socket_t;
#else
typedef int minimk_syscall_socket_t;
#endif

/// Variable containing the AF_INET definition used by this platform.
extern int minimk_syscall_af_inet;

/// Variable containing the AF_INET6 definition used by this platform.
extern int minimk_syscall_af_inet6;

/// Variable containing the invalid socket definition on this platform.
extern minimk_syscall_socket_t minimk_syscall_invalid_socket;

/// Variable containing the platform's POLLIN definition.
extern short minimk_syscall_pollin;

/// Variable containing the platform's POLLOUT definition.
extern short minimk_syscall_pollout;

/// Variable containing the platform's POLLERR definition.
extern short minimk_syscall_pollerr;

/// Variable containing the SOCK_STREAM definition used by this platform.
extern int minimk_syscall_sock_stream;

/// Variable containing the SOCK_DGRAM definition used by this platform.
extern int minimk_syscall_sock_dgram;

MINIMK_BEGIN_DECLS

/// Function to accept a connection on a listening socket.
///
/// This function is thread-safe.
///
/// The client_sock return argument will be set to the invalid socket when the function
/// is invoked and later changed to a valid socket on success.
///
/// The sock argument must be a valid listening socket.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_syscall_accept(minimk_syscall_socket_t *client_sock,
                                     minimk_syscall_socket_t sock) MINIMK_NOEXCEPT;

/// Function to bind a socket to a local address.
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
minimk_error_t minimk_syscall_bind(minimk_syscall_socket_t sock, const char *address,
                                   const char *port) MINIMK_NOEXCEPT;

/// Function to close a socket instance.
///
/// This function is thread-safe.
///
/// The sock parameter will be set to the invalid socket after closing.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_syscall_closesocket(minimk_syscall_socket_t *sock) MINIMK_NOEXCEPT;

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

/// Clears the current errno value before invoking a system call.
void minimk_syscall_clearerrno(void) MINIMK_NOEXCEPT;

/// Returns the portable version of the current errno value after a system call failure.
minimk_error_t minimk_syscall_geterrno(void) MINIMK_NOEXCEPT;

/// Function to get SO_ERROR socket option.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket.
///
/// The error output argument will be set to the socket error value mapped
/// to minimk_error_t. A value of 0 indicates no error (connection succeeded),
/// while nonzero values indicate the specific error that occurred.
///
/// This function is typically used after a nonblocking connect() returns
/// EINPROGRESS to determine the final connection status once the socket
/// becomes writeable.
///
/// The return value is zero on success (getsockopt succeeded) or a nonzero
/// error code on failure (getsockopt failed).
minimk_error_t minimk_syscall_getsockopt_error(minimk_syscall_socket_t sock,
                                               minimk_error_t *error) MINIMK_NOEXCEPT;

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

/// Function that blocks until sockets become readable/writable, a timeout expires
/// or we are interrupted by a signal (thus returning MINIMK_EINTR).
///
/// This function is thread-safe.
///
/// The fds argument points to an array of fds.
///
/// The size is the size of the array (we allow up to UINT16_MAX fds currently).
///
/// The timeout is the number of milliseconds to wait, if positive, zero to
/// avoid blocking, and negative to block until I/O or signal.
///
/// The nready return argument is set to zero when the function is called and
/// contains the number of ready descriptors on success.
///
/// The return value is zero on success or a nonzero error code on failure. Note
/// that success includes the case where no descriptors are ready.
minimk_error_t minimk_syscall_poll(minimk_syscall_pollfd_t *fds, size_t size, int timeout,
                                   size_t *nready) MINIMK_NOEXCEPT;

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

/// Function to set SO_NOSIGPIPE socket option.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket.
///
/// On platforms that support SO_NOSIGPIPE (BSD variants), this function
/// sets the socket option to avoid SIGPIPE when writing to a connection
/// closed by the peer. On other platforms, this function is a no-op.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_syscall_setsockopt_nosigpipe(minimk_syscall_socket_t sock) MINIMK_NOEXCEPT;

/// Function to create a new socket instance.
///
/// This function is thread-safe.
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
minimk_error_t minimk_syscall_socket(minimk_syscall_socket_t *sock, int domain, int type,
                                     int protocol) MINIMK_NOEXCEPT;

/// Function for initializing the socket library and its defines.
///
/// This function is not thread-safe and must be called once before using other
/// socket functions, typically at program startup.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_syscall_socket_init(void) MINIMK_NOEXCEPT;

/// Function to configure a socket as nonblocking.
///
/// This function is thread-safe.
///
/// The sock argument must be a valid socket.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_syscall_socket_setnonblock(minimk_syscall_socket_t sock) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // MINIMK_SYSCALL_H
