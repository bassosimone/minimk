// File: libminimk/stack_linux.hpp
// Purpose: coroutine stack code for linux.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "stack_linux.hpp"

#include <minimk/errno.h> // for minimk_error_t

minimk_error_t minimk_stack_alloc(struct stack *sp) noexcept {
    return __minimk_stack_alloc(sp);
}

minimk_error_t minimk_stack_free(struct stack *sp) noexcept {
    return __minimk_stack_free(sp);
}
