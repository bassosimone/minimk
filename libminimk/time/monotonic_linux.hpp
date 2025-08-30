// File: libminimk/time/monotonic_linux.hpp
// Purpose: linux monotonic timer implementation
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_TIME_MONOTONIC_LINUX_HPP
#define LIBMINIMK_TIME_MONOTONIC_LINUX_HPP

#include "../integer/u64.h"   // for minimk_integer_u64_satmul
#include "../syscall/errno.h" // for minimk_syscall_clearerrno

#include <minimk/assert.h> // for MINIMK_ASSERT
#include <minimk/errno.h>  // for minimk_error_t

#include <stdint.h> // for uint64_t
#include <time.h>   // for clock_gettime

/// Testable implementation of minimk_time_monotonic_now
template <
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(clock_gettime) M_vdso_clock_gettime = clock_gettime>
uint64_t minimk_time_monotonic_now_impl(void) noexcept {
    struct timespec ts = {};
    int clock_gettime_rv = M_vdso_clock_gettime(CLOCK_MONOTONIC, &ts);
    MINIMK_ASSERT(clock_gettime_rv == 0);
    M_minimk_syscall_clearerrno();
    uint64_t now = (uint64_t)ts.tv_sec;
    now = minimk_integer_u64_satmul(now, 1000000000LL);
    now = minimk_integer_u64_satadd(now, (uint64_t)ts.tv_nsec);
    return now;
}

#endif // LIBMINIMK_TIME_MONOTONIC_LINUX_HPP
