// File: include/minimk/trace.h
// Purpose: utility to trace execution
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_TRACE_H
#define MINIMK_TRACE_H

#include <minimk/log.h> // for minimk_log_printf

#include <inttypes.h> // for uint32_t

/// Knob to control whether tracing is enabled.
extern uint32_t minimk_trace_enable;

/// Enable the tracing of syscall events.
#define MINIMK_TRACE_ENABLE_SYSCALL (1 << 0)

/// Enable the tracing of coroutine events.
#define MINIMK_TRACE_ENABLE_COROUTINE (1 << 1)

/// Enable the tracing of scheduler events.
#define MINIMK_TRACE_ENABLE_SCHEDULER (1 << 2)

/// Enable the tracing of socket events.
#define MINIMK_TRACE_ENABLE_SOCKET (1 << 3)

/// Trace syscall events if this tracing has been enabled.
#define MINIMK_TRACE_SYSCALL(fmt, ...)                                                                       \
    do {                                                                                                     \
        if ((minimk_trace_enable & MINIMK_TRACE_ENABLE_SYSCALL) != 0) {                                      \
            minimk_log_printf("  syscall: " fmt, __VA_ARGS__);                                               \
        }                                                                                                    \
    } while (0)

/// Trace coroutine events if this tracing has been enabled.
#define MINIMK_TRACE_COROUTINE(fmt, ...)                                                                     \
    do {                                                                                                     \
        if ((minimk_trace_enable & MINIMK_TRACE_ENABLE_COROUTINE) != 0) {                                    \
            minimk_log_printf("     coro: " fmt, __VA_ARGS__);                                               \
        }                                                                                                    \
    } while (0)

/// Trace scheduler events if this tracing has been enabled.
#define MINIMK_TRACE_SCHEDULER(fmt, ...)                                                                     \
    do {                                                                                                     \
        if ((minimk_trace_enable & MINIMK_TRACE_ENABLE_SCHEDULER) != 0) {                                    \
            minimk_log_printf("    sched: " fmt, __VA_ARGS__);                                               \
        }                                                                                                    \
    } while (0)

/// Trace socket events if this tracing has been enabled.
#define MINIMK_TRACE_SOCKET(fmt, ...)                                                                        \
    do {                                                                                                     \
        if ((minimk_trace_enable & MINIMK_TRACE_ENABLE_SOCKET) != 0) {                                       \
            minimk_log_printf("   socket: " fmt, __VA_ARGS__);                                               \
        }                                                                                                    \
    } while (0)

#endif // MINIMK_TRACE_H
