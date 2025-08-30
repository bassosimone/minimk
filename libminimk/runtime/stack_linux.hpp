// File: libminimk/runtime/stack_linux.hpp
// Purpose: coroutine stack management for linux
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_STACK_LINUX_HPP
#define LIBMINIMK_RUNTIME_STACK_LINUX_HPP

#include "stack.h" // for struct stack

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for minimk_syscall_clearerrno
#include <minimk/trace.h>   // for MINIMK_TRACE

#include <sys/mman.h> // for mmap

#include <stddef.h> // for size_t
#include <stdint.h> // for uintptr_t
#include <unistd.h> // for getpagesize

/// Ensure that the stack is an integral number of pages and possibly equal to a desired size.
static inline size_t minimk_coro_stack_size_impl(size_t page_size) noexcept {
    // We aim to have 64 KiB of coroutine stack plus one guard page.
    static constexpr size_t desired = 64 << 10;

    // However, if the page size is bigger than the stack we want (e.g., if it is 4 MiB), then it
    // does not make sense to micro-allocate since mmap is page aligned anyway.
    if (page_size > desired) {
        return page_size;
    }

    // Compute the mask to align a value to the page size. With 4 KiB (0x01000) this
    // means that we use a 0x00fff mask to align a value to the page size.
    size_t page_mask = (page_size - 1);

    // Round the stack size up to the largest page size multiple. This means that with
    // a 4 KiB page size and a desired value of 65000 (0x0fde8), we perform the
    // following computation:
    //
    //     (0x0fde8 + 0x00fff) & ~0x00fff =
    //          0x10de7        &  0xff000 =
    //          0x10000
    //
    // This is the the smallest multiple of page size that fits the desired size.
    return (desired + page_mask) & ~page_mask;
}

/// Testable implementation of minimk_runtime_stack_alloc
template <
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(getpagesize) M_sys_getpagesize = getpagesize, decltype(mmap) M_sys_mmap = mmap,
        decltype(mprotect) M_sys_mprotect = mprotect, decltype(munmap) M_sys_munmap = munmap>
minimk_error_t minimk_runtime_stack_alloc_impl(struct stack *sp) noexcept {
    // Initialize the total amount of memory to allocate including the guard page.
    int page_size_signed = M_sys_getpagesize();
    MINIMK_ASSERT(page_size_signed >= 0);
    size_t page_size = static_cast<size_t>(page_size_signed);
    size_t coro_stack_size = minimk_coro_stack_size_impl(page_size);
    sp->size = coro_stack_size + page_size;

    // Use mmap to create a memory mapping of the desired size.
    M_minimk_syscall_clearerrno();
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    void *base = M_sys_mmap(nullptr, sp->size, prot, flags, -1, 0);
    if (base == MAP_FAILED) {
        return MINIMK_ENOMEM;
    }
    sp->base = reinterpret_cast<uintptr_t>(base);
    sp->bottom = sp->base + page_size;
    sp->top = sp->base + sp->size;

    // Use mprotect to create a safety guard page.
    M_minimk_syscall_clearerrno();
    if (M_sys_mprotect(base, page_size, PROT_NONE) == -1) {
        M_minimk_syscall_clearerrno();
        M_sys_munmap(base, sp->size);
        return MINIMK_ENOMEM;
    }

    // Let the user know about the stack we have created
    MINIMK_TRACE("trace: allocated new stack<base=0x%llx, size=0x%llx> with layout:\n",
                 static_cast<unsigned long long>(sp->base),
                 static_cast<unsigned long long>(sp->size));
    MINIMK_TRACE("    [0x%llx, 0x%llx) role=guard_page PROT_NONE\n",
                 static_cast<unsigned long long>(sp->base),
                 static_cast<unsigned long long>(sp->base + page_size));
    MINIMK_TRACE("    [0x%llx, 0x%llx) role=coro_stack PROT_READ|PROT_WRITE\n",
                 static_cast<unsigned long long>(sp->base + page_size),
                 static_cast<unsigned long long>(sp->base + sp->size));

    // Return indicating success
    return 0;
}

template <
        decltype(minimk_syscall_clearerrno) M_minimk_syscall_clearerrno = minimk_syscall_clearerrno,
        decltype(minimk_syscall_geterrno) M_minimk_syscall_geterrno = minimk_syscall_geterrno,
        decltype(munmap) M_sys_munmap = munmap>
minimk_error_t minimk_runtime_stack_free_impl(struct stack *sp) noexcept {
    // Unmap the stack from memory
    M_minimk_syscall_clearerrno();
    int rv = M_sys_munmap(reinterpret_cast<void *>(sp->base), sp->size);

    // Let the user know what we have done
    MINIMK_TRACE("trace: freed previously allocated stack<base=0x%llx, size=0x%llx>\n",
                 static_cast<unsigned long long>(sp->base),
                 static_cast<unsigned long long>(sp->size));

    // Clear the stack structure
    sp->base = 0;
    sp->size = 0;

    // Assemble the correct return value
    return (rv == -1) ? M_minimk_syscall_geterrno() : 0;
}

#endif // LIBMINIMK_RUNTIME_STACK_LINUX_HPP
