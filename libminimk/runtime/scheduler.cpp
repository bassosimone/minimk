// File: libminimk/runtime/scheduler.cpp
// Purpose: coroutine scheduler
// SPDX-License-Identifier: GPL-3.0-or-later

#include "scheduler.h"   // for struct scheduler
#include "scheduler.hpp" // for minimk_runtime_scheduler_get_coroutine_slot_impl

#include <minimk/errno.h> // for minimk_error_t

#include <stddef.h> // for size_t

struct coroutine *minimk_runtime_scheduler_get_coroutine_slot( //
        struct scheduler *sched, size_t idx) noexcept {
    return minimk_runtime_scheduler_get_coroutine_slot_impl(sched, idx);
}

minimk_error_t minimk_runtime_scheduler_find_free_coroutine_slot( //
        struct scheduler *sched, struct coroutine **found) noexcept {
    return minimk_runtime_scheduler_find_free_coroutine_slot_impl(sched, found);
}

void minimk_runtime_scheduler_clean_exited_coroutines(struct scheduler *sched) noexcept {
    minimk_runtime_scheduler_clean_exited_coroutines_impl(sched);
}

void minimk_runtime_scheduler_maybe_expire_deadlines(struct scheduler *sched) noexcept {
    minimk_runtime_scheduler_maybe_expire_deadlines_impl(sched);
}

struct coroutine *minimk_runtime_scheduler_pick_runnable( //
        struct scheduler *sched, size_t *fair) noexcept {
    return minimk_runtime_scheduler_pick_runnable_impl(sched, fair);
}

size_t minimk_runtime_scheduler_count_nonnull_coroutines(struct scheduler *sched) noexcept {
    return minimk_runtime_scheduler_count_nonnull_coroutines_impl(sched);
}

void minimk_runtime_scheduler_block_on_poll(struct scheduler *sched) noexcept {
    minimk_runtime_scheduler_block_on_poll_impl(sched);
}

void minimk_runtime_scheduler_switch(struct scheduler *sched) noexcept {
    minimk_runtime_scheduler_switch_impl(sched);
}
