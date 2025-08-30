// File: libminimk/time/monotonic_linux.hpp
// Purpose: linux monotonic timer implementation
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_TIME_MONOTONIC_LINUX_HPP
#define LIBMINIMK_TIME_MONOTONIC_LINUX_HPP

#include "../errno/errno.h" // for minimk_errno_clear
#include "../integer/u64.h" // for minimk_integer_u64_satmul

#include <minimk/assert.h> // for MINIMK_ASSERT
#include <minimk/errno.h>  // for minimk_error_t

#include <stdint.h> // for uint64_t
#include <time.h>   // for clock_gettime

/// Testable implementation of minimk_time_monotonic_now
template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(clock_gettime) __vdso_clock_gettime = clock_gettime>
uint64_t __minimk_time_monotonic_now(void) noexcept {
    struct timespec ts = {.tv_sec = 0, .tv_nsec = 0};
    int clock_gettime_rv = __vdso_clock_gettime(CLOCK_MONOTONIC, &ts);
    MINIMK_ASSERT(clock_gettime_rv == 0);
    __minimk_errno_clear();
    uint64_t now = (uint64_t)ts.tv_sec;
    now = minimk_integer_u64_satmul(now, 1000000000LL);
    now = minimk_integer_u64_satadd(now, (uint64_t)ts.tv_nsec);
    return now;
}

#endif // LIBMINIMK_TIME_MONOTONIC_LINUX_HPP
