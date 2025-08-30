// File: libminimk/runtime/stack.h
// Purpose: coroutine stack management for runtime
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_STACK_H
#define LIBMINIMK_RUNTIME_STACK_H

#include <minimk/cdefs.h>  // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h> // for minimk_error_t

#include <stddef.h> // for size_t
#include <stdint.h> // for uintptr_t

/// Stack allocated by a coroutine.
struct stack {
    /// The point in memory where the guard page starts.
    uintptr_t base;

    /// The totally allocated stack size.
    size_t size;

    /// The top of the stack where we should point the stack pointer.
    uintptr_t top;

    /// The bottom of the stack, where the stack ends.
    uintptr_t bottom;
};

MINIMK_BEGIN_DECLS

/// Allocates a coroutine stack inside the given stack structure.
///
/// Returns zero on success and an error code on failure.
minimk_error_t minimk_runtime_stack_alloc(struct stack *sp) MINIMK_NOEXCEPT;

/// Frees the stack bound to the given stack structure.
///
/// Additionally, zeroes the stack structure.
minimk_error_t minimk_runtime_stack_free(struct stack *sp) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_RUNTIME_STACK_H
