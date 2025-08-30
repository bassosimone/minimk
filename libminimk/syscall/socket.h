// File: libminimk/syscall/socket.h
// Purpose: socket(2) portable wrapper
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_SOCKET_H
#define LIBMINIMK_SYSCALL_SOCKET_H

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

#ifdef _WIN32
#include <stdint.h> // for uintptr_t
#endif

/// Type representing a socket descriptor.
///
/// The type is uintptr_t on Windows and int on POSIX.
#ifdef _WIN32
typedef uintptr_t minimk_syscall_socket_t;
#else
typedef int minimk_syscall_socket_t;
#endif

MINIMK_BEGIN_DECLS

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

MINIMK_END_DECLS

/// Variable containing the invalid socket definition on this platform.
extern minimk_syscall_socket_t minimk_syscall_invalid_socket;

#endif // LIBMINIMK_SYSCALL_SOCKET_H
