// File: libminimk/time/monotonic_linux.cpp
// Purpose: linux monotonic timer implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "monotonic_linux.hpp"

#include <minimk/time.h> // for minimk_time_monotonic_now

uint64_t minimk_time_monotonic_now(void) noexcept {
    return minimk_time_monotonic_now__();
}
