// File: libminimk/time/monotonic.c
// Purpose: linux monotonic timer implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../integer/u64.h" // for minimk_integer_u64_satmul

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_gettime_monotonic
#include <minimk/time.h>    // for minimk_time_monotonic_now

#include <stdint.h> // for uint64_t

uint64_t minimk_time_monotonic_now(void) {
    // Issue the syscall
    uint64_t sec = 0, nsec = 0;
    minimk_error_t rv = minimk_syscall_gettime_monotonic(&sec, &nsec);

    // There's nothing we can do if the call fails
    MINIMK_ASSERT(rv == 0);

    // Convert to nanoseconds which gives us quite a large range
    uint64_t now = sec;
    now = minimk_integer_u64_satmul(now, 1000000000LL);
    now = minimk_integer_u64_satadd(now, nsec);
    return now;
}
