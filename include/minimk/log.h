// File: include/minimk/log.h
// Purpose: logging functionality
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_LOG_H
#define MINIMK_LOG_H

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS

MINIMK_BEGIN_DECLS

/// Format and print the given message using the stderr and then call abort.
///
/// This function is thread safe.
void minimk_log_fatalf(const char *fmt, ...) //
        MINIMK_NOEXCEPT __attribute__((format(printf, 1, 2))) __attribute__((noreturn));

/// Format and print the given message using the stderr.
///
/// This function is thread safe.
void minimk_log_printf(const char *fmt, ...) //
        MINIMK_NOEXCEPT __attribute__((format(printf, 1, 2)));

MINIMK_END_DECLS

#endif // MINIMK_LOG_H
