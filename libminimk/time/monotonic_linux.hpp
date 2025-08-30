// File: libminimk/time/monotonic_linux.hpp
// Purpose: linux monotonic timer implementation
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_TIME_MONOTONIC_LINUX_HPP
#define LIBMINIMK_TIME_MONOTONIC_LINUX_HPP

#include "../integer/u64.h" // for minimk_integer_u64_satmul

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_clearerrno
#include <minimk/trace.h>   // for MINIMK_TRACE

#include <stdint.h> // for uint64_t
#include <time.h>   // for clock_gettime

/// Testable implementation of minimk_time_monotonic_now
template <
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(clock_gettime) M_vdso_clock_gettime = clock_gettime>
uint64_t minimk_time_monotonic_now_impl(void) noexcept {
    // Clear the errno as all good citizens do
    M_minimk_syscall_clearerrno();

    // Issue the system/vdso call
    struct timespec ts = {};
    int clock_gettime_rv = M_vdso_clock_gettime(CLOCK_MONOTONIC, &ts);

    MINIMK_TRACE_SYSCALL("clock_gettime: rv=%d\n", clock_gettime_rv);

    // There's nothing we can do if the call fails
    MINIMK_ASSERT(clock_gettime_rv == 0);

    // Convert to nanoseconds which gives us quite a large range
    uint64_t now = static_cast<uint64_t>(ts.tv_sec);
    now = minimk_integer_u64_satmul(now, 1000000000LL);
    now = minimk_integer_u64_satadd(now, static_cast<uint64_t>(ts.tv_nsec));

    MINIMK_TRACE_SYSCALL("clock_gettime: monotonic_time_now=%lu\n", now);
    return now;
}

#endif // LIBMINIMK_TIME_MONOTONIC_LINUX_HPP
