// File: libminimk/syscall/socket_init_posix.cpp
// Purpose: POSIX minimk_syscall_socket_init implementation
// SPDX-License-Identifier: GPL-3.0-or-later

#include "socket.h" // for minimk_socket_t

#include <minimk/errno.h>   // for minimk_error_t
#include <minimk/syscall.h> // for C function declarations

#include <sys/socket.h> // for AF_INET

int minimk_syscall_af_inet = AF_INET;
int minimk_syscall_af_inet6 = AF_INET6;
int minimk_syscall_sock_stream = SOCK_STREAM;
int minimk_syscall_sock_dgram = SOCK_DGRAM;

int minimk_syscall_invalid_socket = -1;

minimk_error_t minimk_syscall_socket_init(void) {
    return 0;
}
