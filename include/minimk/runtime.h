// File: include/minimk/runtime.h
// Purpose: minimk coorutine cooperative netio runtime.
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_RUNTIME_H
#define MINIMK_RUNTIME_H

#include <minimk/cdefs.h>   // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_socket_t

#include <stddef.h> // for size_t
#include <stdint.h> // for uint64_t

/// Generic opaque handle managed by the runtime.
typedef uint64_t minimk_runtime_handle_t;

/// A socket managed by the runtime is a specific kind of handle.
typedef minimk_runtime_handle_t minimk_runtime_socket_t;

/// Invalid handle value used by the runtime.
#define MINIMK_RUNTIME_INVALID_HANDLE 0

MINIMK_BEGIN_DECLS

/// Creates a coroutine that the runtime will execute.
///
/// The entry argument is the function implementing the coroutine.
///
/// The opaque argument is the argument to pass to the entry function. In case
/// of success, entry takes ownership of the opaque pointer.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_runtime_go(void (*entry)(void *opaque), void *opaque) MINIMK_NOEXCEPT;

/// Blocks executing goroutines until there are goroutines to execute.
///
/// This function must be called at most once usually from the program `main()`.
void minimk_runtime_run(void) MINIMK_NOEXCEPT;

/// Yield the CPU and transfer control to the runtime scheduler.
///
/// This function must be called by a running goroutine.
void minimk_runtime_yield(void) MINIMK_NOEXCEPT;

/// Put the coroutine to sleep for the given amount of nanoseconds.
///
/// A too large number of nanoseconds would be reasonably truncated by the
/// runtime to avoid overflows. You do not actually need to sleep for so much
/// time anyway. We will surely extinguish ourselves before that.
///
/// This function must be called by a running goroutine.
void minimk_runtime_nanosleep(uint64_t nanosec) MINIMK_NOEXCEPT;

/// Put the coroutine to sleep until read would not block or there's a timeout.
///
/// A too large number of nanoseconds would be reasonably truncated by the
/// runtime to avoid overflows. You do not actually need to sleep for so much
/// time anyway. We will surely extinguish ourselves before that.
///
/// Returns zero if read would not block and an error otherwise. Typically, the
/// error is MINIMK_ETIMEDOUT in case of I/O timeout.
minimk_error_t minimk_runtime_suspend_read(minimk_syscall_socket_t sock,
                                           uint64_t nanosec) MINIMK_NOEXCEPT;

/// Like minimk_runtime_suspend_read but for writability.
minimk_error_t minimk_runtime_suspend_write(minimk_syscall_socket_t sock,
                                            uint64_t nanosec) MINIMK_NOEXCEPT;

/// Function to create a new socket instance.
///
/// The sock return argument will be set to MINIMK_RUNTIME_INVALID_HANDLE when the
/// function is invoked and later changed to a valid socket on success.
///
/// The socket structure will refer to a valid socket and its timeouts will
/// be set to UINT64_MAX, which is basically equal to infinite.
///
/// The underlying socket is configured as nonblocking.
///
/// On success, you take ownership of the heap allocated socket.
///
/// The domain MUST be AF_INET or AF_INET6.
///
/// The type MUST be SOCK_STREAM or SOCK_DGRAM.
///
/// The protocol must be zero.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_runtime_socket_create(minimk_runtime_socket_t *sock, int domain, int type,
                                            int protocol) MINIMK_NOEXCEPT;

/// Function to set the read timeout associated with a runtime socket.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_runtime_socket_set_read_timeout(minimk_runtime_socket_t sock,
                                                      uint64_t nanosec) MINIMK_NOEXCEPT;

/// Function to set the write timeout associated with a runtime socket.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_runtime_socket_set_write_timeout(minimk_runtime_socket_t sock,
                                                       uint64_t nanosec) MINIMK_NOEXCEPT;

/// Function to bind a socket to a local address and port.
///
/// The sock argument must be a valid socket created using minimk_runtime_socket_create.
///
/// The address argument must be a valid IPv4 or IPv6 address in numeric format
/// represented as a string (e.g., "127.0.0.1", "::1"). Use "0.0.0.0" for IPv4
/// or "::" for IPv6 to bind to all available interfaces.
///
/// The port argument must be a valid port number represented as a string (e.g., "8080").
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_runtime_socket_bind(minimk_runtime_socket_t sock, const char *address,
                                          const char *port) MINIMK_NOEXCEPT;

/// Function to mark a socket as listening for incoming connections.
///
/// The sock argument must be a valid socket created using minimk_runtime_socket_create
/// and bound using minimk_runtime_socket_bind.
///
/// The backlog argument specifies the maximum length of the queue of pending
/// connections. A value of 128 is typically reasonable for most applications.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_runtime_socket_listen(minimk_runtime_socket_t sock,
                                            int backlog) MINIMK_NOEXCEPT;

/// Function to accept an incoming connection on a listening socket.
///
/// The client_sock return argument will be set to MINIMK_RUNTIME_INVALID_HANDLE when the
/// function is invoked and later changed to a valid socket on success representing the
/// accepted client connection. The underlying client_sock socket will be configured for
/// using nonblocking I/O.
///
/// The sock argument must be a valid socket created using minimk_runtime_socket_create,
/// bound using minimk_runtime_socket_bind, and marked as listening using
/// minimk_runtime_socket_listen.
///
/// The return value is zero on success or a nonzero error code on failure.
///
/// We return MINIMK_ETIMEDOUT when the sock read_timeout expires.
minimk_error_t minimk_runtime_socket_accept(minimk_runtime_socket_t *client_sock,
                                            minimk_runtime_socket_t sock) MINIMK_NOEXCEPT;

/// Function to read bytes from a given socket.
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
///
/// We return MINIMK_ETIMEDOUT when the sock read_timeout expires.
minimk_error_t minimk_runtime_socket_recv(minimk_runtime_socket_t sock, void *data, size_t count,
                                          size_t *nread) MINIMK_NOEXCEPT;

/// Like minimk_runtime_socket_sendall but for receiving.
minimk_error_t minimk_runtime_socket_recvall(minimk_runtime_socket_t sock, void *buf,
                                             size_t count) MINIMK_NOEXCEPT;

/// Function to write bytes to a given socket.
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
///
/// We return MINIMK_ETIMEDOUT when the sock write_timeout expires.
minimk_error_t minimk_runtime_socket_send(minimk_runtime_socket_t sock, const void *data,
                                          size_t count, size_t *nwritten) MINIMK_NOEXCEPT;

/// Like minimk_runtime_socket_send but sends all the content of the buffer unless an error occurs.
///
/// In case of short write, returns error. This is an all-or-nothing operation.
///
/// When interruped by MINIMK_EINTR, this function continues to write relentlessly.
minimk_error_t minimk_runtime_socket_sendall(minimk_runtime_socket_t sock, const void *buf,
                                             size_t count) MINIMK_NOEXCEPT;

/// Function to destroy a socket instance.
///
/// The sock argument must be a valid socket created using minimk_runtime_socket_create.
/// On return, the sock argument is unconditionally set to be MINIMK_RUNTIME_INVALID_HANDLE.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_runtime_socket_destroy(minimk_runtime_socket_t *sock) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // MINIMK_RUNTIME_H
