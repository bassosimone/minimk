// File: libminimk/log/log.c
// Purpose: logging functionality
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/log.h> // for minimk_log_fatalf

#include <stdarg.h> // for va_list
#include <stdio.h>  // for vfprintf
#include <stdlib.h> // for abort

void minimk_log_fatalf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    (void)vfprintf(stderr, fmt, ap);
    va_end(ap);
    abort();
}

void minimk_log_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    (void)vfprintf(stderr, fmt, ap);
    va_end(ap);
}
