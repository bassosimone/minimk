// File: include/minimk/assert.h
// Purpose: like assert but not influenced by NDEBUG
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_ASSERT_H
#define MINIMK_ASSERT_H

#include <minimk/core.h> // for MINIMK_BEGIN_DECLS

MINIMK_BEGIN_DECLS

/// Internal implementation of `MINIMK_ASSERT`.
void __minimk_assert(int should_abort, const char *fmt, ...) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

/// Non-maskable assertion that causes abort on failure.
#define MINIMK_ASSERT(expr) __minimk_assert(!(expr), "assertion failed: %s (%s:%d)\n", #expr, __FILE__, __LINE__)

#endif // MINIMK_ASSERT_H
