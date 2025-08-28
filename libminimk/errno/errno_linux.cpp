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
