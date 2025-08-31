// File: libminimk/runtime/runtime.cpp
// Purpose: cooperative coroutine runtime implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../integer/u64.h" // for minimk_integer_u64_satadd

#include "coroutine.h" // for struct coroutine
#include "scheduler.h" // for struct scheduler
#include "switch.h"    // for minimk_switch

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/cdefs.h>   // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/runtime.h> // for minimk_runtime_run
#include <minimk/syscall.h> // for minimk_syscall_*
#include <minimk/time.h>    // for minimk_time_monotonic_now

#include <stddef.h> // for size_t
#include <stdint.h> // for uintptr_t

/// Global scheduler to use
static scheduler s0;

minimk_error_t minimk_runtime_go(void (*entry)(void *opaque), void *opaque) noexcept {
    return minimk_runtime_scheduler_coroutine_create(&s0, entry, opaque);
}

void minimk_runtime_run(void) noexcept {
    minimk_runtime_scheduler_run(&s0);
}

void minimk_runtime_yield(void) noexcept {
    minimk_runtime_scheduler_coroutine_yield(&s0);
}

void minimk_runtime_nanosleep(uint64_t nanosec) noexcept {
    minimk_runtime_scheduler_coroutine_suspend_timer(&s0, nanosec);
}

minimk_error_t minimk_runtime_suspend_read(minimk_syscall_socket_t sock,
                                           uint64_t nanosec) MINIMK_NOEXCEPT {
    return minimk_runtime_scheduler_coroutine_suspend_io(&s0, sock, minimk_syscall_pollin, nanosec);
}

minimk_error_t minimk_runtime_suspend_write(minimk_syscall_socket_t sock,
                                            uint64_t nanosec) MINIMK_NOEXCEPT {
    return minimk_runtime_scheduler_coroutine_suspend_io(&s0, sock, minimk_syscall_pollout,
                                                         nanosec);
}
