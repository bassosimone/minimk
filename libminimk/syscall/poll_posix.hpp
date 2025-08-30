// File: libminimk/syscall/poll_posix.hpp
// Purpose: POSIX poll(2) wrapper
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SYSCALL_POLL_POSIX_HPP
#define LIBMINIMK_SYSCALL_POLL_POSIX_HPP

#include "errno.h" // for minimk_syscall_geterrno

#include "poll.h" // for minimk_syscall_pollfd

#include <minimk/errno.h> // for minimk_error_t

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

    // Clear errno and issues the system call.
    M_minimk_syscall_clearerrno();
    int rv = M_sys_poll(fds, static_cast<nfds_t>(size), timeout);

    // Determine whether poll succeeded.
    int success = (rv >= 0);

    // Set nready according to whether we succeeded.
    *nready = (success) ? static_cast<size_t>(rv) : 0;

    // Set return value according to whether we succeded.
    return (success) ? 0 : M_minimk_syscall_geterrno();
}

#endif // LIBMINIMK_SYSCALL_POLL_POSIX_HPP
