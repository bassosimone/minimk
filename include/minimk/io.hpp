// File: include/minimk/io.hpp
// Purpose: I/O library
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_IO_HPP
#define MINIMK_IO_HPP

#include <minimk/assert.h> // for MINIMK_ASSERT
#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_send

#include <stddef.h> // for size_t

/// Write as much data as possible from the underlying buffer.
///
/// This function automatically resumes sending when interrupted by a signal.
template <decltype(minimk_socket_send) __minimk_socket_send>
minimk_error_t minimk_io_writeall(minimk_socket_t sock, const void *buf, size_t count, size_t *total) noexcept {
    for (*total = 0; *total < count;) {
        size_t actual = 0;
        minimk_error_t rv = __minimk_socket_send(sock, (const char *)buf + *total, count - *total, &actual);
        if (rv == MINIMK_EINTR) {
            continue;
        }
        if (rv != 0) {
            return (*total > 0) ? 0 : rv;
        }
        *total += actual;
    }
    return 0;
}

/// Read as much data as possible into the given buffer.
///
/// This function automatically resumes receiving when interrupted by a signal.
template <decltype(minimk_socket_recv) __minimk_socket_recv>
minimk_error_t minimk_io_readall(minimk_socket_t sock, void *buf, size_t count, size_t *total) noexcept {
    for (*total = 0; *total < count;) {
        size_t actual = 0;
        minimk_error_t rv = __minimk_socket_recv(sock, (char *)buf + *total, count - *total, &actual);
        if (rv == MINIMK_EINTR) {
            continue;
        }
        if (rv != 0) {
            return (*total > 0) ? 0 : rv;
        }
        MINIMK_ASSERT(actual > 0); // minimk_socket_recv uses MINIMK_EOF on EOF
        *total += actual;
    }
    return 0;
}

#endif // MINIMK_IO_HPP
