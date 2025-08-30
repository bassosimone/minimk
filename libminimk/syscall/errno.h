// File: libminimk/syscall/errno.h
// Purpose: portable syscall errno functions
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_ERRNO_H
#define LIBMINIMK_SYSCALL_ERRNO_H

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

MINIMK_BEGIN_DECLS

/// Clears the current errno value before invoking a system call.
void minimk_syscall_clearerrno(void) MINIMK_NOEXCEPT;

/// Returns the portable version of the current errno value after a system call failure.
minimk_error_t minimk_syscall_geterrno(void) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_SYSCALL_ERRNO_H

