// File: include/minimk/syscall.h
// Purpose: syscall defines and wrappers
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_SYSCALL_H
#define MINIMK_SYSCALL_H

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

MINIMK_BEGIN_DECLS

/// Function for initializing the socket library and its defines.
///
/// This function is not thread-safe and must be called once before using other
/// socket functions, typically at program startup.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_syscall_socket_init(void) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

/// Variable containing the AF_INET definition used by this platform.
extern int minimk_syscall_af_inet;

/// Variable containing the AF_INET6 definition used by this platform.
extern int minimk_syscall_af_inet6;

/// Variable containing the SOCK_STREAM definition used by this platform.
extern int minimk_syscall_sock_stream;

/// Variable containing the SOCK_DGRAM definition used by this platform.
extern int minimk_syscall_sock_dgram;

#endif // MINIMK_SYSCALL_H
