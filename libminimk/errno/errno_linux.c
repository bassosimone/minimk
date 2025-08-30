// File: libminimk/errno/errno_linux.c
// Purpose: errno handling for linux
// SPDX-License-Identifier: GPL-3.0-or-later

#include "errno.h" // for minimk_errno_get

#include <minimk/errno.h> // for minimk_errno_clear

#include <errno.h> // for errno

minimk_error_t minimk_errno_get(void) {
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

void minimk_errno_clear(void) {
    errno = 0;
}

const char *minimk_errno_name(minimk_error_t code) {
    switch (code) {
    case 0:
        return "SUCCESS";

    case MINIMK_EAGAIN:
        return "EAGAIN";

    case MINIMK_EUNKNOWN:
        return "EUNKNOWN";

    case MINIMK_ECONNREFUSED:
        return "ECONNREFUSED";

    case MINIMK_ECONNRESET:
        return "ECONNRESET";

    case MINIMK_EFAULT:
        return "EFAULT";

    case MINIMK_EHOSTUNREACH:
        return "EHOSTUNREACH";

    case MINIMK_EINPROGRESS:
        return "EINPROGRESS";

    case MINIMK_EINTR:
        return "EINTR";

    case MINIMK_EINVAL:
        return "EINVAL";

    case MINIMK_EACCES:
        return "EACCES";

    case MINIMK_EADDRINUSE:
        return "EADDRINUSE";

    case MINIMK_EAFNOSUPPORT:
        return "EAFNOSUPPORT";

    case MINIMK_ENETUNREACH:
        return "ENETUNREACH";

    case MINIMK_ENOBUFS:
        return "ENOBUFS";

    case MINIMK_EPROTONOSUPPORT:
        return "EPROTONOSUPPORT";

    case MINIMK_ETIMEDOUT:
        return "ETIMEDOUT";

    case MINIMK_ENOMEM:
        return "ENOMEM";

    case MINIMK_EOF:
        return "EOF";

    case MINIMK_EBADF:
        return "EBADF";

    case MINIMK_EMFILE:
        return "EMFILE";

    default:
        return "UNKNOWN";
    }
}
