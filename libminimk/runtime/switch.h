// File: libminimk/runtime/switch.h
// Purpose: coroutine context-switching for runtime
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_SWITCH_H
#define LIBMINIMK_RUNTIME_SWITCH_H

#include <minimk/cdefs.h> // for MINIMK_BEGIN_DECLS

#include <stdint.h> // for uintptr_t

MINIMK_BEGIN_DECLS

/// Switches from the current stack to a new stack.
///
/// The current stack pointer (after saving callee-saved registers) will be
/// stored at the location pointed to by old_stack_ptr.
///
/// The new_stack_ptr must point to a valid saved stack state from a previous
/// switch or from minimk_runtime_init_coro_stack initialization.
void minimk_runtime_switch(uintptr_t *old_stack_ptr, uintptr_t new_stack_ptr) MINIMK_NOEXCEPT;

/// Initializes a coroutine stack for first-time execution.
///
/// The stack_ptr will be set to point to the initialized stack state.
///
/// The stack_top should point to the top of the allocated stack memory.
///
/// The trampoline is the address of the function that will be called when the coroutine first runs.
///
/// The sched address is the address of the scheduler owning us.
void minimk_runtime_init_coro_stack(uintptr_t *stack_ptr, uintptr_t stack_top, uintptr_t trampoline,
                                    uintptr_t sched) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_RUNTIME_SWITCH_H
