// File: examples/runtime/02_coroutine_sleep.c
// Purpose: execute coroutines that sleep and print messages
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/runtime.h> // for minimk_runtime_go
#include <minimk/trace.h>   // for minimk_trace_enable

#include <stdio.h> // for fprintf

static void every_s(void *opaque) {
    (void)opaque;
    for (size_t idx = 0; idx < 4; idx++) {
        minimk_runtime_nanosleep(1000000000);
        fprintf(stderr, "1 s\n");
    }
}

static void every_100ms(void *opaque) {
    (void)opaque;
    for (size_t idx = 0; idx < 16; idx++) {
        minimk_runtime_nanosleep(250000000);
        fprintf(stderr, "250 ms\n");
    }
}

static void init(void *opaque) {
    (void)opaque;
    fprintf(stderr, "init!\n");
    minimk_runtime_go(every_s, NULL);
    minimk_runtime_go(every_100ms, NULL);
}

int main(void) {
    minimk_trace_enable |= MINIMK_TRACE_ENABLE_COROUTINE;
    minimk_trace_enable |= MINIMK_TRACE_ENABLE_SCHEDULER;
    minimk_trace_enable |= MINIMK_TRACE_ENABLE_SYSCALL;

    minimk_runtime_go(init, NULL);
    minimk_runtime_run();
}
