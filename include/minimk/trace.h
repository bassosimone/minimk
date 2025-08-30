// File: include/minimk/trace.h
// Purpose: utility to trace execution
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_TRACE_H
#define MINIMK_TRACE_H

#include <minimk/log.h> // for minimk_log_printf

#include <inttypes.h> // for uint32_t

/// Deprecated: we should use specific channels instead.
#ifdef MINIMK_ENABLE_TRACE
#define MINIMK_TRACE_IS_ENABLED 1
#else
#define MINIMK_TRACE_IS_ENABLED 0
#endif

/// Traces execution and prints specific values.
///
/// Deprecated: we should use specific channels instead.
#define MINIMK_TRACE(fmt, ...)                                                                     \
    do {                                                                                           \
        if (MINIMK_TRACE_IS_ENABLED) {                                                             \
            minimk_log_printf(fmt, __VA_ARGS__);                                                   \
        }                                                                                          \
    } while (0)

/// Knob to control whether tracing is enabled.
extern uint32_t minimk_trace_enable;

/// Enable the tracing of syscall events.
#define MINIMK_TRACE_ENABLE_SYSCALL (1 << 0)

/// Trace syscall events if this tracing has been enabled.
#define MINIMK_TRACE_SYSCALL(fmt, ...)                                                             \
    do {                                                                                           \
        if ((minimk_trace_enable & MINIMK_TRACE_ENABLE_SYSCALL) != 0) {                            \
            minimk_log_printf("trace: " fmt, __VA_ARGS__);                                         \
        }                                                                                          \
    } while (0)

#endif // MINIMK_TRACE_H
