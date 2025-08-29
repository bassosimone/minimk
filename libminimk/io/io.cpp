// File: libminimk/io/io.cpp
// Purpose: I/O library
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/io.h>   // for minimk_io_socket_writeall
#include <minimk/io.hpp> // for minimk_io_writeall

minimk_error_t minimk_io_socket_writeall(minimk_socket_t sock, const void *buf, size_t count, size_t *total) noexcept {
    return minimk_io_writeall<minimk_socket_send>(sock, buf, count, total);
}

minimk_error_t minimk_io_socket_readall(minimk_socket_t sock, void *buf, size_t count, size_t *total) noexcept {
    return minimk_io_readall<minimk_socket_recv>(sock, buf, count, total);
}
