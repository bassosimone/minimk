// File: libminimk/runtime/trace.h
// Purpose: utility to trace execution
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_TRACE_H
#define LIBMINIMK_RUNTIME_TRACE_H

#include <minimk/log.h> // for minimk_log_printf

#ifdef MINIMK_ENABLE_TRACE
#define MINIMK_TRACE__ 1
#else
#define MINIMK_TRACE__ 0
#endif

/// Traces execution and prints specific values
#define MINIMK_TRACE(fmt, ...)                                                                     \
    do {                                                                                           \
        if (MINIMK_TRACE__) {                                                                      \
            minimk_log_printf(fmt, ##__VA_ARGS__);                                                 \
        }                                                                                          \
    } while (0)

#endif // LIBMINIMK_RUNTIME_TRACE_H
