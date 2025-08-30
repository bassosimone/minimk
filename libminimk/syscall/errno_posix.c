// File: libminimk/syscall/errno_posix.c
// Purpose: syscall errno functions on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../errno/errno_posix.h" // for minimk_errno_map

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_geterrno

#include <errno.h> // for errno

minimk_error_t minimk_syscall_geterrno(void) {
    return minimk_errno_map(errno);
}

void minimk_syscall_clearerrno(void) {
    errno = 0;
}
