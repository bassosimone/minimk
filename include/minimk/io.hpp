// File: include/minimk/io.hpp
// Purpose: I/O library
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_IO_HPP
#define MINIMK_IO_HPP

#include <minimk/assert.h> // for MINIMK_ASSERT
#include <minimk/errno.h>  // for minimk_error_t

#include <stddef.h> // for size_t

/// Write as much data as possible from the underlying buffer.
///
/// This function automatically resumes sending when interrupted by a signal.
///
/// M_T_send must return MINIMK_EINVAL when count is zero.
///
/// M_T_send must return nwritten > 0 when its return code is zero.
template <typename T, minimk_error_t (*M_T_send)(T, const void *, size_t, size_t *)>
minimk_error_t minimk_io_writeall(T sock, const void *vbuf, size_t count) noexcept {
    const char *buf = static_cast<const char *>(vbuf);

    for (size_t total = 0; total < count;) {
        size_t nwritten = 0;
        size_t amount = count - total;

#pragma clang unsafe_buffer_usage begin
        minimk_error_t rv = M_T_send(sock, buf + total, amount, &nwritten);
#pragma clang unsafe_buffer_usage end

        if (rv == MINIMK_EINTR) {
            continue;
        }
        if (rv != 0) {
            return rv; // all-or-nothing semantics
        }

        MINIMK_ASSERT(nwritten > 0); // enforces documented assumptions
        MINIMK_ASSERT(nwritten <= amount);
        total += nwritten;
    }
    return 0;
}

/// Read as much data as possible into the given buffer.
///
/// This function automatically resumes receiving when interrupted by a signal.
///
/// M_T_recv must return MINIMK_EINVAL when count is zero.
///
/// M_T_recv must return MINIMK_EOF on EOF.
///
/// M_T_recv must return nread > 0 when its return code is zero.
template <typename T, minimk_error_t (*M_T_recv)(T, void *, size_t, size_t *)>
minimk_error_t minimk_io_readall(T sock, void *vbuf, size_t count) noexcept {
    char *buf = static_cast<char *>(vbuf);

    for (size_t total = 0; total < count;) {
        size_t nread = 0;
        size_t amount = count - total;

#pragma clang unsafe_buffer_usage begin
        minimk_error_t rv = M_T_recv(sock, buf + total, amount, &nread);
#pragma clang unsafe_buffer_usage end

        if (rv == MINIMK_EINTR) {
            continue;
        }
        if (rv != 0) {
            return rv; // all-or-nothing semantics
        }
        MINIMK_ASSERT(nread > 0); // enforces documented assumptions
        MINIMK_ASSERT(nread <= amount);
        total += nread;
    }
    return 0;
}

#endif // MINIMK_IO_HPP
