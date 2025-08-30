// File: libminimk/socket/socket_linux.cpp
// Purpose: socket library implemented for linux
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../io/io.hpp"

#include <minimk/syscall.h> // for minimk_syscall_*

#include "socket.h" // for minimk_socket_t

#include <minimk/errno.h> // for minimk_error_t

minimk_error_t minimk_socket_sendall(minimk_syscall_socket_t sock, const void *buf,
                                     size_t count) noexcept {
    return minimk_io_writeall_impl<minimk_syscall_socket_t, minimk_syscall_send>(sock, buf, count);
}

minimk_error_t minimk_socket_recvall(minimk_syscall_socket_t sock, void *buf,
                                     size_t count) noexcept {
    return minimk_io_readall_impl<minimk_syscall_socket_t, minimk_syscall_recv>(sock, buf, count);
}
