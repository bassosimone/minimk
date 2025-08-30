// File: libminimk/runtime/coroutine.h
// Purpose: coroutine specific code
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_COROUTINE_H
#define LIBMINIMK_RUNTIME_COROUTINE_H

#include "stack.h" // for struct stack

#include <minimk/cdefs.h>   // for MINIMK_BEGIN_DECLS
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_*

#include <stdint.h> // for uintptr_t

/// Unused coroutine slot.
#define CORO_NULL 0

/// Coroutine can run immediately.
#define CORO_RUNNABLE 1

/// Coroutine has terminated.
#define CORO_EXITED 2

/// Coroutine is blocked awaiting for a deadline.
#define CORO_BLOCKED_ON_TIMER 3

/// Coroutine is blocked awaiting for a socket.
#define CORO_BLOCKED_ON_IO 4

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

    /// Management of the blocked state.
    uint64_t deadline;
    minimk_syscall_socket_t sock;
    short events;
    short revents;

} __attribute__((aligned(16)));

MINIMK_BEGIN_DECLS

/// Initializes the given coroutine struct with the given entry and opaque pointer.
///
/// This function is thread safe.
///
/// The coro argument is the coroutine struct to initialize.
///
/// The trampoline argument is a pointer to the coroutine trampoline function.
///
/// The entry argument is the function that the coroutine should execute.
///
/// The opaque argument is the argument for entry.
///
/// On success, this function transfers ownership of opaque to the coroutine.
///
/// Returns zero on success and a nonzero error code on failure.
minimk_error_t minimk_runtime_coroutine_init(struct coroutine *coro, void (*trampoline)(void),
                                             void (*entry)(void *opaque),
                                             void *opaque) MINIMK_NOEXCEPT;

/// Releases the associated resources and zeroes the coroutine.
void minimk_runtime_coroutine_finish(struct coroutine *coro) MINIMK_NOEXCEPT;

/// Resumes the given coroutine if was sleeping on a timer or I/O and the current time
/// and/or the I/O conditions in revents indicate that it should be resumed.
void minimk_runtime_coroutine_maybe_resume(struct coroutine *coro, uint64_t now,
                                           short revents) MINIMK_NOEXCEPT;

/// Function that aborts if the given coroutine stack pointer is not valid.
void minimk_runtime_coroutine_validate_stack_pointer( //
        const char *context, struct coroutine *coro) MINIMK_NOEXCEPT;

/// Parks the current coroutine until the given timeout expires.
void minimk_runtime_coroutine_suspend_io(struct coroutine *coro, minimk_syscall_socket_t sock,
                                         short events, uint64_t nanosec) MINIMK_NOEXCEPT;

/// Resume the current coroutine after it suspended on I/O.
minimk_error_t minimk_runtime_coroutine_resume_io(struct coroutine *coro,
                                                  minimk_syscall_socket_t sock,
                                                  short events) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // LIBMINIMK_RUNTIME_COROUTINE_H
