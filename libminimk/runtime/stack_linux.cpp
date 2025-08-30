// File: libminimk/runtime/stack_linux.cpp
// Purpose: coroutine stack management for linux
// SPDX-License-Identifier: GPL-3.0-or-later

#include "stack_linux.hpp"

#include <minimk/errno.h> // for minimk_error_t

minimk_error_t minimk_runtime_stack_alloc(struct stack *sp) noexcept {
    return minimk_runtime_stack_alloc_impl(sp);
}

minimk_error_t minimk_runtime_stack_free(struct stack *sp) noexcept {
    return minimk_runtime_stack_free_impl(sp);
}
