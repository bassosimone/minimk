// File: libminimk/stack.h
// Purpose: coroutine stack code.
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_STACK_H
#define LIBMINIMK_STACK_H

#include <minimk/core.h>  // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

#include <stddef.h> // for size_t

/// Stack allocated by a coroutine.
struct stack {
    uintptr_t base;
    size_t size;
};

MINIMK_BEGIN_DECLS

/// Allocates a coroutine stack inside the given stack structure.
///
/// Returns zero on success and an error code on failure.
minimk_error_t minimk_stack_alloc(struct stack *sp) MINIMK_NOEXCEPT;

/// Frees the stack bound to the given stack structure.
///
/// Additionally, zeroes the stack structure.
minimk_error_t minimk_stack_free(struct stack *sp) MINIMK_NOEXCEPT;

/// Returns the pointer to the stack top.
///
/// This function assumes the stack grows downwards, which should
/// be the case on arm64 and amd64 and possibly other platforms.
static inline uintptr_t minimk_stack_top(struct stack *sp) MINIMK_NOEXCEPT {
    return sp->base + sp->size;
}

MINIMK_END_DECLS

#endif // LIBMINIMK_STACK_H
