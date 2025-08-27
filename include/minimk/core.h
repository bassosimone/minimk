// File: include/minimk/core.h
// Purpose: core library
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_CORE_H
#define MINIMK_CORE_H

/// Macro identifying the beginning of C declarations.
#ifdef __cplusplus
#define MINIMK_BEGIN_DECLS extern "C" {
#else
#define MINIMK_BEGIN_DECLS /* Nothing */
#endif

/// Macro identifying the end of C declarations.
#ifdef __cplusplus
#define MINIMK_END_DECLS }
#else
#define MINIMK_END_DECLS /* Nothing */
#endif

/// Macro identifying that a function does not throw exceptions.
#ifdef __cplusplus
#define MINIMK_NOEXCEPT noexcept
#else
#define MINIMK_NOEXCEPT /* Nothing */
#endif

#endif // MINIMK_CORE_H
