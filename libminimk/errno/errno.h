// File: libminimk/errno/errno.h
// Purpose: portable functions to manipulate errno
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_ERRNO_ERRNO_H
#define LIBMINIMK_ERRNO_ERRNO_H

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

MINIMK_BEGIN_DECLS

/// Clears the current errno value before invoking a system call.
void minimk_errno_clear(void) MINIMK_NOEXCEPT;

/// Returns the portable version of the current errno value after a system call failure.
minimk_error_t minimk_errno_get(void) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_ERRNO_ERRNO_H
