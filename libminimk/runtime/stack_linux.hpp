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

template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(getpagesize) __sys_getpagesize = getpagesize,
          decltype(mmap) __sys_mmap = mmap,
          decltype(mprotect) __sys_mprotect = mprotect,
          decltype(munmap) __sys_munmap = munmap>
minimk_error_t __minimk_runtime_stack_alloc(struct stack *sp) noexcept {
    // Initialize the total amount of memory to allocate including the guard page.
    size_t page_size = __sys_getpagesize();
    size_t stack_size = 64 << 10;
    sp->size = stack_size + page_size;

    // Use mmap to create a memory mapping of the desired size.
    __minimk_errno_clear();
    void *base = __sys_mmap(nullptr, sp->size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base == MAP_FAILED) {
        return MINIMK_ENOMEM;
    }
    sp->base = (uintptr_t)base;

    // Use mprotect to create a safety guard page.
    __minimk_errno_clear();
    if (__sys_mprotect(base, page_size, PROT_NONE) == -1) {
        __minimk_errno_clear();
        __sys_munmap(base, sp->size);
        return MINIMK_ENOMEM;
    }
    return 0;
}

template <decltype(minimk_errno_clear) __minimk_errno_clear = minimk_errno_clear,
          decltype(minimk_errno_get) __minimk_errno_get = minimk_errno_get,
          decltype(munmap) __sys_munmap = munmap>
minimk_error_t __minimk_runtime_stack_free(struct stack *sp) noexcept {
    __minimk_errno_clear();
    int rv = __sys_munmap((void *)sp->base, sp->size);
    sp->base = 0;
    sp->size = 0;
    return (rv == -1) ? __minimk_errno_get() : 0;
}

#endif // LIBMINIMK_RUNTIME_STACK_LINUX_HPP
