// File: include/minimk/cdefs.h
// Purpose: C compiler definitions
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_CDEFS_H
#define MINIMK_CDEFS_H

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

/// Macros for handling unsafe buffer usage warnings.
#ifdef __clang__
#define MINIMK_UNSAFE_BUFFER_USAGE_BEGIN _Pragma("clang unsafe_buffer_usage begin")
#define MINIMK_UNSAFE_BUFFER_USAGE_END _Pragma("clang unsafe_buffer_usage end")
#else
#define MINIMK_UNSAFE_BUFFER_USAGE_BEGIN /* Nothing */
#define MINIMK_UNSAFE_BUFFER_USAGE_END   /* Nothing */
#endif

/// Macro to always inline templates. In minimk, templates are
/// just an implementation detail for testability.
#ifdef __clang__
#define MINIMK_ALWAYS_INLINE __attribute__((always_inline))
#else
#define MINIMK_ALWAYS_INLINE /* Nothing */
#endif

#endif // MINIMK_CDEFS_H
