// File: examples/runtime/00_coroutine_hello.c
// Purpose: execute a coroutine printing hello world.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/runtime.h> // for minimk_runtime_go

#include <stdio.h> // for fprintf

static void hello(void *opaque) {
    (void)opaque;
    fprintf(stderr, "Hello, world!\n");
}

int main(void) {
    minimk_runtime_go(hello, NULL);
    minimk_runtime_run();
}
