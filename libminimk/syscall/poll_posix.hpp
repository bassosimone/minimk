// File: libminimk/syscall/poll_posix.hpp
// Purpose: POSIX poll(2) wrapper
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_POLL_POSIX_HPP
#define LIBMINIMK_SYSCALL_POLL_POSIX_HPP

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_geterrno
#include <minimk/time.h>    // for minimk_time_monotonic_now
#include <minimk/trace.h>   // for MINIMK_TRACE_SYSCALL

#include <poll.h>   // for poll
#include <stddef.h> // for offsetof
#include <stdint.h> // for UINT16_MAX

/// Testable minimk_syscall_poll implementation.
template <
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
        decltype(poll) M_sys_poll = poll>
minimk_error_t minimk_syscall_poll_impl(minimk_syscall_pollfd_t *fds, size_t size, int timeout,
                                        size_t *nready) noexcept {
    // As documented, camp the maximum number of descriptors.
    size = (size <= UINT16_MAX) ? size : UINT16_MAX;

    // Ensure the cast from a clamped size to nfds_t holds.
    static_assert(sizeof(nfds_t) >= sizeof(uint16_t), "nfds_t must be larger than uint16_t");

    // Log that we're about to invoke the syscall
    MINIMK_TRACE_SYSCALL("poll: size=%zu\n", size);
    MINIMK_TRACE_SYSCALL("poll: timeout=%d\n", timeout);
    MINIMK_TRACE_SYSCALL("poll: t0=%lu\n", minimk_time_monotonic_now());

    // Clear errno and issues the system call.
    M_minimk_syscall_clearerrno();
    int rv = M_sys_poll(fds, static_cast<nfds_t>(size), timeout);

    // Determine whether poll succeeded.
    int success = (rv >= 0);

    // Set nready according to whether we succeeded.
    *nready = (success) ? static_cast<size_t>(rv) : 0;

    // Assign the result of the syscall
    minimk_error_t res = (success) ? 0 : M_minimk_syscall_geterrno();

    // Log the results of invoking the syscall
    MINIMK_TRACE_SYSCALL("poll: result=%s\n", minimk_errno_name(res));
    MINIMK_TRACE_SYSCALL("poll: nready=%zu\n", *nready);
    MINIMK_TRACE_SYSCALL("poll: t1=%lu\n", minimk_time_monotonic_now());

    // Return the result
    return res;
}

#endif // LIBMINIMK_SYSCALL_POLL_POSIX_HPP
