// File: libminimk/runtime/trace.h
// Purpose: utility to trace execution
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_TRACE_H
#define LIBMINIMK_RUNTIME_TRACE_H

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS

MINIMK_BEGIN_DECLS

/// Internal implementation of MINIMK_TRACE
void minimk_trace__(const char *fmt, ...) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#ifdef MINIMK_ENABLE_TRACE
#define MINIMK_TRACE__ 1
#else
#define MINIMK_TRACE__ 0
#endif

/// Traces execution and prints specific values
#define MINIMK_TRACE(fmt, ...)                                                                                         \
    do {                                                                                                               \
        if (MINIMK_TRACE__) {                                                                                          \
            minimk_trace__(fmt, ##__VA_ARGS__);                                                                        \
        }                                                                                                              \
    } while (0)

#endif // LIBMINIMK_RUNTIME_TRACE_H
