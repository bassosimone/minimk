// File: include/minimk/runtime.h
// Purpose: coroutine runtime.
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_RUNTIME_H
#define MINIMK_RUNTIME_H

#include <minimk/core.h>  // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

#include <stdint.h> // for uint64_t

MINIMK_BEGIN_DECLS

/// Creates a coroutine that the runtime will execute.
///
/// The entry argument is the function implementing the coroutine.
///
/// The opaque argument is the argument to pass to the entry function. In case
/// of success, entry takes ownership of the opaque pointer.
///
/// The return value is zero on success or a nonzero error code on failure.
minimk_error_t minimk_runtime_go(void (*entry)(void *opaque), void *opaque) MINIMK_NOEXCEPT;

/// Blocks executing goroutines until there are goroutines to execute.
///
/// This function must be called at most once usually from the program `main()`.
void minimk_runtime_run(void) MINIMK_NOEXCEPT;

/// Yield the CPU and transfer control to the runtime scheduler.
///
/// This function must be called by a running goroutine.
void minimk_runtime_yield(void) MINIMK_NOEXCEPT;

/// Put the coroutine to sleep for the given amount of nanoseconds.
///
/// A too large number of nanoseconds would be reasonably truncated by the
/// runtime to avoid overflows. You do not actually need to sleep for so much
/// time anyway. We will surely extinguish ourselves before that.
///
/// This function must be called by a running goroutine.
void minimk_runtime_nanosleep(uint64_t nanosec) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // MINIMK_RUNTIME_H
