// File: libminimk/runtime/stack_linux.hpp
// Purpose: coroutine stack management for linux
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_STACK_LINUX_HPP
#define LIBMINIMK_RUNTIME_STACK_LINUX_HPP

#include "stack.h" // for struct stack

#include <minimk/errno.h> // for minimk_error_t

#include <sys/mman.h> // for mmap

#include <stddef.h> // for size_t
#include <unistd.h> // for getpagesize

#include "trace.h" // for MINIMK_TRACE

/// Ensure that the stack is an integral number of pages and possibly equal to a desired size.
static inline size_t __minimk_coro_stack_size(size_t page_size) noexcept {
    size_t desired = 1 << 16;
    if (desired < page_size) {
        return page_size;
    }
    size_t page_offset_mask = (page_size - 1);
    return desired & ~page_offset_mask;
}

/// Testable implementation of minimk_runtime_stack_alloc
template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(getpagesize) __sys_getpagesize = getpagesize,
          decltype(mmap) __sys_mmap = mmap,
          decltype(mprotect) __sys_mprotect = mprotect,
          decltype(munmap) __sys_munmap = munmap>
minimk_error_t __minimk_runtime_stack_alloc(struct stack *sp) noexcept {
    // Initialize the total amount of memory to allocate including the guard page.
    size_t page_size = __sys_getpagesize();
    size_t coro_stack_size = __minimk_coro_stack_size(page_size);
    sp->size = coro_stack_size + page_size;

    // Use mmap to create a memory mapping of the desired size.
    __minimk_errno_clear();
    void *base = __sys_mmap(nullptr, sp->size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base == MAP_FAILED) {
        return MINIMK_ENOMEM;
    }
    sp->base = (uintptr_t)base;
    sp->bottom = sp->base + page_size;
    sp->top = sp->base + sp->size;

    // Use mprotect to create a safety guard page.
    __minimk_errno_clear();
    if (__sys_mprotect(base, page_size, PROT_NONE) == -1) {
        __minimk_errno_clear();
        __sys_munmap(base, sp->size);
        return MINIMK_ENOMEM;
    }

    // Let the user know about the stack we have created
    MINIMK_TRACE("trace: allocated new stack<base=0x%llx, size=0x%llx> with layout:\n",
                 (unsigned long long)sp->base,
                 (unsigned long long)sp->size);
    MINIMK_TRACE("    [0x%llx, 0x%llx) role=guard_page PROT_NONE\n",
                 (unsigned long long)sp->base,
                 (unsigned long long)sp->base + page_size);
    MINIMK_TRACE("    [0x%llx, 0x%llx) role=coro_stack PROT_READ|PROT_WRITE\n",
                 (unsigned long long)sp->base + page_size,
                 (unsigned long long)sp->base + sp->size);

    // Return indicating success
    return 0;
}

template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(minimk_errno_get) __minimk_errno_get = minimk_errno_get,
          decltype(munmap) __sys_munmap = munmap>
minimk_error_t __minimk_runtime_stack_free(struct stack *sp) noexcept {
    // Unmap the stack from memory
    __minimk_errno_clear();
    int rv = __sys_munmap((void *)sp->base, sp->size);

    // Let the user know what we have done
    MINIMK_TRACE("trace: freed previously allocated stack<base=0x%llx, size=0x%llx>\n",
                 (unsigned long long)sp->base,
                 (unsigned long long)sp->size);

    // Clear the stack structure
    sp->base = 0;
    sp->size = 0;

    // Assemble the correct return value
    return (rv == -1) ? __minimk_errno_get() : 0;
}

#endif // LIBMINIMK_RUNTIME_STACK_LINUX_HPP
