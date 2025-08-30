// File: libminimk/errno/errno.c
// Purpose: portable errno related code
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/errno.h> // for minimk_error_name

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
