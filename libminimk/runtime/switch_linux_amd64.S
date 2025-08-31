    // File: libminimk/runtime/switch_linux_amd64.S
    // Purpose: coroutine context-switching for runtime on linux amd64
    // SPDX-License-Identifier: GPL-3.0-or-later

    // void minimk_runtime_switch(
    //  uintptr_t *old_stack_ptr /* = rdi */,
    //  uintptr_t  new_stack_ptr /* = rsi */
    // );
    .text
    .globl minimk_runtime_switch
    .type minimk_runtime_switch, @function
minimk_runtime_switch:
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

    .size minimk_runtime_switch, .-minimk_runtime_switch

    // void minimk_runtime_init_coro_stack(
    //  uintptr_t *stack_ptr  /* = rdi */,
    //  uintptr_t  stack_top  /* = rsi */,
    //  uintptr_t  trampoline /* = rdx */,
    //  uintptr_t  sched      /* = rcx */
    // );
    .globl minimk_runtime_init_coro_stack
    .type minimk_runtime_init_coro_stack, @function
minimk_runtime_init_coro_stack:
    // Save the current stack pointer while we mess with the new stack
    movq %rsp, %rax
    
    // Temporarily switch to new coroutine stack
    movq %rsi, %rsp

    // Warning: the current setup where we push 8 things on the
    // stack ensures it is aligned on a 16 bytes boundary. Adding
    // more registers should ensure to preserve this alignment
    // through padding, if necessary. Otherwise, we'll segfault
    // immediately on the first SIMD instruction.

    // Push the scheduler address to the stack so that the trampoline
    // knows the scheduler that owns the coroutine.
    pushq %rcx  // sched
    
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

    .size minimk_runtime_init_coro_stack, .-minimk_runtime_init_coro_stack

    // void minimk_runtime_asm_trampoline(void);
    .globl minimk_runtime_asm_trampoline
    .type minimk_runtime_asm_trampoline, @function
    .extern minimk_runtime_scheduler_coroutine_main
minimk_runtime_asm_trampoline:
    // We are called after switch through ret, so the top of our
    // stack is the scheduler's address.
    movq (%rsp), %rdi

    // We need to create a fake rbp, which actually has the
    // side effect of aligning the stack to 16 bytes.
    pushq $0   // rbp

    // Invoke the C trampoline
    call minimk_runtime_scheduler_coroutine_main
