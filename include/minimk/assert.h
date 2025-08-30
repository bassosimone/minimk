// File: include/minimk/assert.h
// Purpose: like assert but not influenced by NDEBUG
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_ASSERT_H
#define MINIMK_ASSERT_H

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS

#include <stdarg.h> // for va_list
#include <stdio.h>  // for vfprintf
#include <stdlib.h> // for abort

MINIMK_BEGIN_DECLS

/// Internal inline function invoked by MINIMK_ASSERT.
static inline void minimk_assert_abort__(const char *fmt, ...) MINIMK_NOEXCEPT {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    abort();
}

MINIMK_END_DECLS

/// Non-maskable assertion that causes abort on failure.
#define MINIMK_ASSERT(expr)                                                                        \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            minimk_assert_abort__("assertion failed: %s (%s:%d)\n", #expr, __FILE__, __LINE__);    \
        }                                                                                          \
    } while (0)

#endif // MINIMK_ASSERT_H
