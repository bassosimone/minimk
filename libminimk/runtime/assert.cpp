// File: libminimk/runtime/assert.cpp
// Purpose: like assert but not influenced by NDEBUG
// SPDX-License-Identifier: GPL-3.0-or-later

#include "assert.h"

#include <stdarg.h> // for va_list
#include <stdio.h>  // for vfprintf
#include <stdlib.h> // for abort

void __minimk_assert(int should_abort, const char *fmt, ...) noexcept {
    if (should_abort) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        abort();
    }
}
