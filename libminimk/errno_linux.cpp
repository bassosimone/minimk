// File: libminimk/errno_linux.cpp
// Purpose: linux errnos
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/errno.h> // for minimk_errno_clear

#include <errno.h> // for errno

void minimk_errno_clear(void) noexcept {
    errno = 0;
}

minimk_error_t minimk_errno_get(void) noexcept {
    switch (errno) {
    case 0:
        return MINIMK_SUCCESS;

    case EFAULT:
        return MINIMK_EFAULT;

    case EINVAL:
        return MINIMK_EINVAL;

    case EACCES:
        return MINIMK_EACCES;

    case EADDRINUSE:
        return MINIMK_EADDRINUSE;

    case EAFNOSUPPORT:
        return MINIMK_EAFNOSUPPORT;

    case EAGAIN:
        return MINIMK_EAGAIN;

    case ECONNREFUSED:
        return MINIMK_ECONNREFUSED;

    case ECONNRESET:
        return MINIMK_ECONNRESET;

    case EHOSTUNREACH:
        return MINIMK_EHOSTUNREACH;

    case EINPROGRESS:
        return MINIMK_EINPROGRESS;

    case EINTR:
        return MINIMK_EINTR;

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

    default:
        return MINIMK_EUNKNOWN;
    }
}
