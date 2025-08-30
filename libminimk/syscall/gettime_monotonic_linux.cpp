// File: libminimk/syscall/gettime_monotonic_linux.cpp
// Purpose: linux monotonic timer implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gettime_monotonic_linux.hpp" // for minimk_syscall_gettime_monotonic_impl

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_gettime_monotonic

minimk_error_t minimk_syscall_gettime_monotonic(uint64_t *sec, uint64_t *nsec) noexcept {
    return minimk_syscall_gettime_monotonic_impl(sec, nsec);
}
