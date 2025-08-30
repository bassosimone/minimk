// File: include/minimk/socket.h
// Purpose: socket library public API (init and constants only)
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_SOCKET_H
#define MINIMK_SOCKET_H

#include <minimk/core.h>  // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

MINIMK_BEGIN_DECLS

/// Function for initializing the platform-specific socket library.
///
/// This function is not thread-safe and should be called once before using other
/// socket functions, typically at program startup.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_socket_init(void) MINIMK_NOEXCEPT;

/// Function returning the AF_INET value used by the platform.
///
/// This function is thread-safe.
///
/// The return value is AF_INET.
int minimk_socket_af_inet(void) MINIMK_NOEXCEPT;

/// Function returning the AF_INET6 value used by the platform.
///
/// This function is thread-safe.
///
/// The return value is AF_INET6.
int minimk_socket_af_inet6(void) MINIMK_NOEXCEPT;

/// Function returning the SOCK_STREAM value used by the platform.
///
/// This function is thread-safe.
///
/// The return value is SOCK_STREAM.
int minimk_socket_sock_stream(void) MINIMK_NOEXCEPT;

/// Function returning the SOCK_DGRAM value used by the platform.
///
/// This function is thread-safe.
///
/// The return value is SOCK_DGRAM.
int minimk_socket_sock_dgram(void) MINIMK_NOEXCEPT;


MINIMK_END_DECLS

#endif // MINIMK_SOCKET_H
