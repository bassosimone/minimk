// File: libminimk/runtime/trace.c
// Purpose: utility to trace execution
// SPDX-License-Identifier: GPL-3.0-or-later

#include "trace.h" // for minimk_trace__

#include <stdarg.h> // for va_list
#include <stdio.h>  // for stderr

void minimk_trace__(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}
