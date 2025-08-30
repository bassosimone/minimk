// File: include/minimk/assert.h
// Purpose: like assert but not influenced by NDEBUG
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_ASSERT_H
#define MINIMK_ASSERT_H

#include <minimk/log.h> // for minimk_log_fatalf

/// Non-maskable assertion that causes abort on failure.
#define MINIMK_ASSERT(expr)                                                                        \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            minimk_log_fatalf("assertion failed: %s (%s:%d)\n", #expr, __FILE__, __LINE__);        \
        }                                                                                          \
    } while (0)

#endif // MINIMK_ASSERT_H
