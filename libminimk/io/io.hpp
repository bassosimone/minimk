// File: libminimk/io/io.hpp
// Purpose: I/O library
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_IO_IO_HPP
#define LIBMINIMK_IO_IO_HPP

#include <minimk/assert.h> // for MINIMK_ASSERT
#include <minimk/errno.h>  // for minimk_error_t

#include <stddef.h> // for size_t

/// Write as much data as possible from the underlying buffer.
///
/// This function automatically resumes sending when interrupted by a signal.
template <typename T, minimk_error_t (*T_send__)(T, const void *, size_t, size_t *)>
minimk_error_t minimk_io_writeall__(T sock, const void *buf, size_t count) noexcept {
    for (size_t total = 0; total < count;) {
        size_t nwritten = 0;
        minimk_error_t rv = T_send__(sock, (const char *)buf + total, count - total, &nwritten);
        if (rv == MINIMK_EINTR) {
            continue;
        }
        if (rv != 0) {
            return rv; // all-or-nothing semantics
        }
        total += nwritten;
    }
    return 0;
}

/// Read as much data as possible into the given buffer.
///
/// This function automatically resumes receiving when interrupted by a signal.
template <typename T, minimk_error_t (*T_recv__)(T, void *, size_t, size_t *)>
minimk_error_t minimk_io_readall__(T sock, void *buf, size_t count) noexcept {
    for (size_t total = 0; total < count;) {
        size_t nread = 0;
        minimk_error_t rv = T_recv__(sock, (char *)buf + total, count - total, &nread);
        if (rv == MINIMK_EINTR) {
            continue;
        }
        if (rv != 0) {
            return rv; // all-or-nothing semantics
        }
        MINIMK_ASSERT(nread > 0); // minimk_syscall_recv uses MINIMK_EOF on EOF
        total += nread;
    }
    return 0;
}

#endif // LIBMINIMK_IO_IO_HPP
