// File: libminimk/runtime/trace.h
// Purpose: utility to trace execution
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_TRACE_H
#define LIBMINIMK_RUNTIME_TRACE_H

#include <minimk/core.h> // for MINIMK_BEGIN_DECLS

MINIMK_BEGIN_DECLS

/// Internal implementation of MINIMK_TRACE
void __minimk_trace(const char *fmt, ...) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#ifdef MINIMK_ENABLE_TRACE
#define __MINIMK_TRACE 1
#else
#define __MINIMK_TRACE 0
#endif

/// Traces execution and prints specific values
#define MINIMK_TRACE(fmt, ...)                                                                                         \
    do {                                                                                                               \
        if (__MINIMK_TRACE) {                                                                                          \
            __minimk_trace(fmt, __VA_ARGS__);                                                                          \
        }                                                                                                              \
    } while (0)

#endif // LIBMINIMK_RUNTIME_TRACE_H
