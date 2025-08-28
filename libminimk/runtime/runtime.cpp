// File: libminimk/runtime/runtime.cpp
// Purpose: cooperative coroutine runtime implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/runtime.h> // for minimk_runtime_run

#include <assert.h> // for assert
#include <stddef.h> // for size_t
#include <stdint.h> // for uintptr_t
#include <string.h> // for memset

#include "stack.h"  // for struct stack
#include "switch.h" // for minimk_switch

/// The coroutine slot is unused.
#define CORO_NULL 0

/// The coroutine can run immediately.
#define CORO_RUNNABLE 1

/// The coroutine has finished.
#define CORO_EXITED 2

/// Portable coroutine state.
///
/// We align this structure to safely memset it to zero on arm64.
struct coroutine {
    /// Saved stack pointer.
    uintptr_t sp;

    /// Allocated coroutine stack.
    struct stack stack;

    /// Entry point.
    void (*entry)(void *opaque);
    void *opaque;

    /// State.
    unsigned long state;
} __attribute__((aligned(16)));

/// Maximum number of coroutines we can create.
#define MAX_COROS 16

/// Slots for coroutines we manage.
static coroutine coroutines[MAX_COROS];

/// Pointer to currently running coroutine.
static coroutine *current;

/// Stack pointer used by the scheduler.
static uintptr_t scheduler_sp;

/// Find a free coroutine slot or return a nonzero error.
static minimk_error_t find_free_coroutine_slot(coroutine **found) noexcept {
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        coroutine *coro = &coroutines[idx];
        if (coro->state != CORO_NULL) {
            continue;
        }
        *found = coro;
        return 0;
    }
    return MINIMK_EAGAIN;
}

/// Trampoline for starting to execute a coroutine.
static void coro_trampoline(void) {
    // Ensure that we're in the coroutine world.
    assert(current != nullptr);

    // Transfer control to the coroutine entry, which in turn may
    // voluntarily yield the control to other coroutines.
    current->entry(current->opaque);

    // Mark the coroutine as exited and the scheduler will
    // take care of freeing the allocated resources.
    current->state = CORO_EXITED;

    // Transfer the control back to the scheduler.
    minimk_runtime_yield();

    // We should never reach this line.
    assert(false);
}

/// Initialize the given coroutine or return a nonzero error.
static minimk_error_t init_coroutine(coroutine *coro, void (*entry)(void *opaque), void *opaque) noexcept {
    // Zero initialize the whole coroutine
    //
    // If we're not aligned, this may backfire on arm64
    memset(coro, 0, sizeof(*coro));

    // Initialize the entry
    coro->entry = entry;
    coro->opaque = opaque;

    // Allocate a stack for the coroutine
    minimk_error_t rv = minimk_runtime_stack_alloc(&coro->stack);
    if (rv != 0) {
        return rv;
    }

    // Use assembly to synthesize the stack frame
    minimk_runtime_init_coro_stack(&coro->sp, minimk_runtime_stack_top(&coro->stack), (uintptr_t)coro_trampoline);

    // Mark as ready to run
    coro->state = CORO_RUNNABLE;
    return 0;
}

/// Free all resources and mark the coroutine slot as empty.
static void destroy_coroutine(coroutine *coro) {
    (void)minimk_runtime_stack_free(&coro->stack);
    memset(coro, 0, sizeof(*coro)); // The zero state implies empty slot
}

minimk_error_t minimk_runtime_go(void (*entry)(void *opaque), void *opaque) noexcept {
    // 1. find an available coroutine slot
    coroutine *coro = nullptr;
    auto rv = find_free_coroutine_slot(&coro);
    if (rv != 0) {
        return rv;
    }

    // 2. initialize the coroutine slot
    if ((rv = init_coroutine(coro, entry, opaque)) != 0) {
        return rv;
    }

    // 3. declare success
    return 0;
}

/// Frees resources used by all the exited coroutines.
static void sched_clean_exited(void) {
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        auto coro = &coroutines[idx];
        if (coro->state == CORO_EXITED) {
            destroy_coroutine(coro);
            continue;
        }
    }
}

/// Select the first runnable coroutine using a fair algorithm.
static coroutine *sched_pick_runnable(size_t *fair) {
    // note that wrap is not UB for size_t
    for (size_t idx = 0; idx < MAX_COROS; idx++) {
        auto coro = &coroutines[(*fair) % MAX_COROS];
        (*fair)++;
        if (coro->state == CORO_RUNNABLE) {
            return coro;
        }
    }
    return nullptr;
}

void minimk_runtime_run(void) noexcept {
    // Ensure we are not yet inside the coroutine world.
    assert(current == nullptr);

    // Continue until we're out of coroutines.
    for (size_t fair = 0;;) {
        // Check whether there are coroutines that need cleanup.
        sched_clean_exited();

        // Fairly select the first runnable coroutine.
        current = sched_pick_runnable(&fair);

        // If there are no runnable coroutines, we're outta here.
        if (current == nullptr) {
            break;
        }

        // Switch to current for a while.
        minimk_runtime_switch(&scheduler_sp, current->sp);

        // We're now inside the scheduler again.
        current = nullptr;
    }
}

void minimk_runtime_yield(void) noexcept {
    // Ensure we're inside the coroutine world.
    assert(current != nullptr);

    // Manually switch back to the scheduler stack.
    minimk_runtime_switch(&current->sp, scheduler_sp);
}
