// File: examples/time/00_monotonic.c
// Purpose: read the monotonic clock
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/time.h> // for minimk_time_monotonic_now

#include <stdint.h> // for uint64_t
#include <stdio.h>  // for fprintf

int main(void) {
    uint64_t first = minimk_time_monotonic_now();
    fprintf(stderr, "%llu\n", (unsigned long long)first);

    uint64_t second = minimk_time_monotonic_now();
    fprintf(stderr, "%llu\n", (unsigned long long)second);

    uint64_t third = minimk_time_monotonic_now();
    fprintf(stderr, "%llu\n", (unsigned long long)third);
}
