// File: examples/runtime/01_coroutine_pingpong.c
// Purpose: execute coroutines that yield and print messages
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/runtime.h> // for minimk_runtime_go

#include <stdio.h> // for fprintf

static void ping(void *opaque) {
    (void)opaque;
    for (size_t idx = 0; idx < 10; idx++) {
        fprintf(stderr, ">>> ping!\n");
        minimk_runtime_yield();
    }
}

static void pong(void *opaque) {
    (void)opaque;
    for (size_t idx = 0; idx < 10; idx++) {
        fprintf(stderr, "<<< pong!\n");
        minimk_runtime_yield();
    }
}

static void init(void *opaque) {
    (void)opaque;
    fprintf(stderr, "init!\n");
    minimk_runtime_go(ping, NULL);
    minimk_runtime_go(pong, NULL);
}

int main(void) {
    minimk_runtime_go(init, NULL);
    minimk_runtime_run();
}
