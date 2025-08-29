// File: libminimk/errno/errno_linux.cpp
// Purpose: errno handling for linux
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/errno.h> // for minimk_errno_clear

#include <errno.h> // for errno

minimk_error_t minimk_errno_get(void) noexcept {
    return (minimk_error_t)errno;
}

void minimk_errno_clear(void) noexcept {
    errno = 0;
}

const char *minimk_errno_name(minimk_error_t code) noexcept {
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

    default:
        return "UNKNOWN";
    }
}
