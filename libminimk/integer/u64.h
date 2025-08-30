// File: libminimk/integer/u64.h
// Purpose: routines for safely operating with uint64_t
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_INTEGER_U64_H
#define LIBMINIMK_INTEGER_U64_H

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS

#include <inttypes.h> // for uint64_t

MINIMK_BEGIN_DECLS

/// Return the result of the addition if possible, otherwise UINT64_MAX.
static inline uint64_t minimk_integer_u64_satadd(uint64_t left, uint64_t right) MINIMK_NOEXCEPT {
    return (left <= UINT64_MAX - right) ? (left + right) : UINT64_MAX;
}

/// Return the result of the multiplication if possible, otherwise UINT64_MAX.
static inline uint64_t minimk_integer_u64_satmul(uint64_t left, uint64_t right) MINIMK_NOEXCEPT {
    return (left <= UINT64_MAX / right) ? (left * right) : UINT64_MAX;
}

MINIMK_END_DECLS

#endif // LIBMINIMK_INTEGER_U64_H
