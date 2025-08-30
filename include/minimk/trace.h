// File: include/minimk/trace.h
// Purpose: utility to trace execution
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_TRACE_H
#define MINIMK_TRACE_H

#include <minimk/log.h> // for minimk_log_printf

#ifdef MINIMK_ENABLE_TRACE
#define MINIMK_TRACE_IS_ENABLED 1
#else
#define MINIMK_TRACE_IS_ENABLED 0
#endif

/// Traces execution and prints specific values
#define MINIMK_TRACE(fmt, ...)                                                                     \
    do {                                                                                           \
        if (MINIMK_TRACE_IS_ENABLED) {                                                             \
            minimk_log_printf(fmt, __VA_ARGS__);                                                   \
        }                                                                                          \
    } while (0)

#endif // MINIMK_TRACE_H
