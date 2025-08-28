// File: libminimk/runtime/trace.cpp
// Purpose: utility to trace execution
// SPDX-License-Identifier: GPL-3.0-or-later

#include "trace.h" // for __minimk_trace

#include <stdarg.h> // for va_list
#include <stdio.h>  // for stderr

void __minimk_trace(const char *fmt, ...) noexcept {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}
