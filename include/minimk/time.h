// File: include/minimk/time.h
// Purpose: time library
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_TIME_H
#define MINIMK_TIME_H

#include <minimk/core.h> // for MINIMK_BEGIN_DECLS

#include <stdint.h> // for int64_t

MINIMK_BEGIN_DECLS

/// Return the nanoseconds elapsed since the zero of the monotonic clock.
///
/// Calls abort if we cannot get a monotonic clock reading.
///
/// Avoids overflow in the unlikely case when there is an overflow.
uint64_t minimk_time_monotonic_now(void) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // MINIMK_TIME_H
