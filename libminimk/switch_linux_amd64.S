    // File: libminimk/switch_linux_amd64.S
    // Purpose: coroutine context-switching for linux amd64
    // SPDX-License-Identifier: GPL-3.0-or-later

    // void minimk_switch(
    //  uintptr_t *old_stack_ptr /* = rdi */,
    //  uintptr_t  new_stack_ptr /* = rsi */
    // );
    .text
    .globl minimk_switch
    .type minimk_switch, @function
minimk_switch:
    // Save callee-saved registers on current stack
    pushq %rbx
    pushq %rbp
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    
    // Save current stack pointer (points to saved frame)
    movq %rsp, (%rdi)
    
    // Switch to new stack
    movq %rsi, %rsp
    
    // Restore callee-saved registers from new stack
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbp
    popq %rbx

    // Return (the return address is already on the stack on amd64)
    ret

    .size minimk_switch, .-minimk_switch

    // void minimk_init_coro_stack(
    //  uintptr_t *stack_ptr  /* = rdi */,
    //  uintptr_t  stack_top  /* = rsi */,
    //  uintptr_t  trampoline /* = rdx */
    // );
    .globl minimk_init_coro_stack
    .type minimk_init_coro_stack, @function
minimk_init_coro_stack:
    // Save the current stack pointer while we mess with the new stack
    movq %rsp, %rax
    
    // Temporarily switch to new coroutine stack
    movq %rsi, %rsp
    
    // Push trampoline address as the address to return to
    pushq %rdx
    
    // Push zeroed callee-saved registers (as if we saved them)
    pushq $0    // rbx
    pushq $0    // rbp
    pushq $0    // r12
    pushq $0    // r13
    pushq $0    // r14
    pushq $0    // r15
    
    // Save new stack pointer (points to saved frame)
    movq %rsp, (%rdi)
    
    // Restore the original stack pointer
    movq %rax, %rsp
    ret

    .size minimk_init_coro_stack, .-minimk_init_coro_stack
