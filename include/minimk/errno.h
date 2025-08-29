// File: include/minimk/errno.h
// Purpose: errno defines
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_ERRNO_H
#define MINIMK_ERRNO_H

#include <minimk/core.h> // for MINIMK_BEGIN_DECLS

#include <stdint.h> // for uint32_t

/// Type representing an error code.
typedef uint32_t minimk_error_t;

/// No error occurred.
#define MINIMK_SUCCESS 0

/// Unknown error.
#define MINIMK_EUNKNOWN 1

/// Bad address.
#define MINIMK_EFAULT 2

/// Invalid argument.
#define MINIMK_EINVAL 3

/// Permission denied.
#define MINIMK_EACCES 4

/// Address already in use.
#define MINIMK_EADDRINUSE 5

/// Address family not supported.
#define MINIMK_EAFNOSUPPORT 6

/// Resource temporarily unavailable.
#define MINIMK_EAGAIN 7

/// Operation would block.
#define MINIMK_EWOULDBLOCK MINIMK_EAGAIN

/// Connection refused.
#define MINIMK_ECONNREFUSED 8

/// Connection reset by peer.
#define MINIMK_ECONNRESET 9

/// Host is unreachable.
#define MINIMK_EHOSTUNREACH 10

/// Operation in progress.
#define MINIMK_EINPROGRESS 11

/// Interrupted system call.
#define MINIMK_EINTR 12

/// Network is unreachable.
#define MINIMK_ENETUNREACH 13

/// No buffer space available.
#define MINIMK_ENOBUFS 14

/// Protocol not supported.
#define MINIMK_EPROTONOSUPPORT 15

/// Connection timed out.
#define MINIMK_ETIMEDOUT 16

/// Out of memory.
#define MINIMK_ENOMEM 17

/// We reached the end of the stream.
#define MINIMK_EOF 18

MINIMK_BEGIN_DECLS

/// Clears the current errno value before invoking a system call.
void minimk_errno_clear(void) MINIMK_NOEXCEPT;

/// Returns the portable version of the current errno value after a system call failure.
minimk_error_t minimk_errno_get(void) MINIMK_NOEXCEPT;

/// Return the name of the errno value (i.e., MINIMK_EINTR => "EINTR").
const char *minimk_errno_name(minimk_error_t code) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // MINIMK_ERRNO_H
