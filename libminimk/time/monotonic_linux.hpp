// File: libminimk/time/monotonic_linux.hpp
// Purpose: linux monotonic timer implementation
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_TIME_MONOTONIC_LINUX_HPP
#define LIBMINIMK_TIME_MONOTONIC_LINUX_HPP

#include "../integer/u64.h"    // for minimk_integer_u64_satmul
#include "../syscall/errno.h" // for minimk_syscall_clearerrno

#include <minimk/assert.h> // for MINIMK_ASSERT
#include <minimk/errno.h>  // for minimk_error_t

#include <stdint.h> // for uint64_t
#include <time.h>   // for clock_gettime

/// Testable implementation of minimk_time_monotonic_now
template <decltype(minimk_syscall_clearerrno) minimk_syscall_clearerrno__ = minimk_syscall_clearerrno,
          decltype(clock_gettime) vdso_clock_gettime__ = clock_gettime>
uint64_t minimk_time_monotonic_now__(void) noexcept {
    struct timespec ts = {.tv_sec = 0, .tv_nsec = 0};
    int clock_gettime_rv = vdso_clock_gettime__(CLOCK_MONOTONIC, &ts);
    MINIMK_ASSERT(clock_gettime_rv == 0);
    minimk_syscall_clearerrno__();
    uint64_t now = (uint64_t)ts.tv_sec;
    now = minimk_integer_u64_satmul(now, 1000000000LL);
    now = minimk_integer_u64_satadd(now, (uint64_t)ts.tv_nsec);
    return now;
}

#endif // LIBMINIMK_TIME_MONOTONIC_LINUX_HPP
