// File: libminimk/runtime/runtime.h
// Purpose: private runtime functions for internal use
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_RUNTIME_H
#define LIBMINIMK_RUNTIME_RUNTIME_H

#include "../socket/socket.h" // for minimk_socket_t

#include <minimk/cdefs.h>  // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

#include <stdint.h> // for uint64_t

MINIMK_BEGIN_DECLS

/// Put the coroutine to sleep until read would not block or there's a timeout.
///
/// A too large number of nanoseconds would be reasonably truncated by the
/// runtime to avoid overflows. You do not actually need to sleep for so much
/// time anyway. We will surely extinguish ourselves before that.
///
/// Returns zero if read would not block and an error otherwise. Typically, the
/// error is MINIMK_ETIMEDOUT in case of I/O timeout.
minimk_error_t minimk_runtime_suspend_read(minimk_socket_t sock, uint64_t nanosec) MINIMK_NOEXCEPT;

/// Like minimk_runtime_suspend_read but for writability.
minimk_error_t minimk_runtime_suspend_write(minimk_socket_t sock, uint64_t nanosec) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_RUNTIME_RUNTIME_H
