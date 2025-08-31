// File: libminimk/syscall/gettime_monotonic_linux.hpp
// Purpose: linux monotonic timer implementation
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_GETTIME_MONOTONIC_LINUX_HPP
#define LIBMINIMK_SYSCALL_GETTIME_MONOTONIC_LINUX_HPP

#include "../cast/static.hpp" // for CAST_ULL

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_clearerrno
#include <minimk/trace.h>   // for MINIMK_TRACE_SYSCALL

#include <stdint.h> // for uint64_t
#include <time.h>   // for clock_gettime

/// Testable implementation of minimk_syscall_gettime_monotonic
template <decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
          decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
          decltype(clock_gettime) M_vdso_clock_gettime = clock_gettime>
minimk_error_t minimk_syscall_gettime_monotonic_impl(uint64_t *sec, uint64_t *nsec) noexcept {
    // Issue the system/vdso clock_gettime call
    M_minimk_syscall_clearerrno();
    struct timespec ts = {};
    int rv = M_vdso_clock_gettime(CLOCK_MONOTONIC, &ts);

    // Assign the return arguments
    *sec = (rv == 0) ? static_cast<uint64_t>(ts.tv_sec) : 0;
    *nsec = (rv == 0) ? static_cast<uint64_t>(ts.tv_nsec) : 0;

    // Assign the actual result
    minimk_error_t res = (rv != 0) ? M_minimk_syscall_geterrno() : 0;

    // Log the results of the system call
    MINIMK_TRACE_SYSCALL("clock_gettime: result=%s\n", minimk_errno_name(res));
    MINIMK_TRACE_SYSCALL("clock_gettime: sec=%llu\n", CAST_ULL(*sec));
    MINIMK_TRACE_SYSCALL("clock_gettime: nsec=%llu\n", CAST_ULL(*nsec));

    // Return to the caller.
    return res;
}

#endif // LIBMINIMK_SYSCALL_GETTIME_MONOTONIC_LINUX_HPP
