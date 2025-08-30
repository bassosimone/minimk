// File: libminimk/socket/socket_linux.cpp
// Purpose: socket library implemented for linux
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../io/io.hpp"
#include "../syscall/recv_posix.h" // for minimk_syscall_recv
#include "../syscall/send_posix.h" // for minimk_syscall_send

#include "socket.h"         // for minimk_socket_t
#include "socket_linux.hpp" // for __minimk_socket_create

#include <minimk/errno.h> // for minimk_error_t

minimk_error_t minimk_socket_sendall(minimk_socket_t sock, const void *buf, size_t count) noexcept {
    return __minimk_io_writeall<minimk_socket_t, minimk_syscall_send>(sock, buf, count);
}

minimk_error_t minimk_socket_recvall(minimk_socket_t sock, void *buf, size_t count) noexcept {
    return __minimk_io_readall<minimk_socket_t, minimk_syscall_recv>(sock, buf, count);
}
