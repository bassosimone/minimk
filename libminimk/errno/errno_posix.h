// File: libminimk/errno/errno_posix.h
// Purpose: POSIX-specific errno mapping functions
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_ERRNO_ERRNO_POSIX_H
#define LIBMINIMK_ERRNO_ERRNO_POSIX_H

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

MINIMK_BEGIN_DECLS

/// Maps a POSIX errno value to a minimk_error_t.
///
/// This function is thread-safe.
///
/// The posix_errno argument is any POSIX errno value (e.g., EAGAIN, EINVAL).
///
/// The return value is the corresponding minimk_error_t value, or MINIMK_EUNKNOWN
/// for unmapped errno values.
minimk_error_t minimk_errno_map(int posix_errno) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_ERRNO_ERRNO_POSIX_H
