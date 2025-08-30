// File: libminimk/syscall/errno_posix.c
// Purpose: syscall errno functions on POSIX
// SPDX-License-Identifier: GPL-3.0-or-later

#include "errno.h" // for minimk_syscall_geterrno

#include <minimk/errno.h> // for minimk_error_t

#include <errno.h> // for errno

// Ensure that our assumption about EAGAIN and EWOULDBLOCK holds
_Static_assert(EAGAIN == EWOULDBLOCK, "EAGAIN assumed to be == EWOULDBLOCK");

minimk_error_t minimk_syscall_geterrno(void) {
    switch (errno) {
    case 0:
        return 0;

    case EAGAIN:
        return MINIMK_EAGAIN;

    case ECONNREFUSED:
        return MINIMK_ECONNREFUSED;

    case ECONNRESET:
        return MINIMK_ECONNRESET;

    case EFAULT:
        return MINIMK_EFAULT;

    case EHOSTUNREACH:
        return MINIMK_EHOSTUNREACH;

    case EINPROGRESS:
        return MINIMK_EINPROGRESS;

    case EINTR:
        return MINIMK_EINTR;

    case EINVAL:
        return MINIMK_EINVAL;

    case EACCES:
        return MINIMK_EACCES;

    case EADDRINUSE:
        return MINIMK_EADDRINUSE;

    case EAFNOSUPPORT:
        return MINIMK_EAFNOSUPPORT;

    case ENETUNREACH:
        return MINIMK_ENETUNREACH;

    case ENOBUFS:
        return MINIMK_ENOBUFS;

    case EPROTONOSUPPORT:
        return MINIMK_EPROTONOSUPPORT;

    case ETIMEDOUT:
        return MINIMK_ETIMEDOUT;

    case ENOMEM:
        return MINIMK_ENOMEM;

    case EBADF:
        return MINIMK_EBADF;

    case EMFILE:
        return MINIMK_EMFILE;

    default:
        return MINIMK_EUNKNOWN;
    }
}

void minimk_syscall_clearerrno(void) {
    errno = 0;
}
