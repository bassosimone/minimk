// File: libminimk/cast/static.hpp
// Purpose: helpers to make static casting more compact.
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_CAST_STATIC_H
#define LIBMINIMK_CAST_STATIC_H

/// Syntactic sugar for writing a static cast to `void *`.
#define CAST_VOID_P(value) static_cast<void *>(value)

/// Syntactic sugar for writing a static cast to `unsigned long long`.
#define CAST_ULL(value) static_cast<unsigned long long>(value)

/// Syntactic sugar for writing a static cast to `unsigned`.
#define CAST_U(value) static_cast<unsigned>(value)

#endif // LIBMINIMK_CAST_STATIC_H
