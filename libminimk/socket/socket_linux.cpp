// File: libminimk/socket/socket_linux.cpp
// Purpose: socket library implemented for linux
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../io/io.hpp"

#include "socket.h"         // for minimk_socket_t
#include "socket_linux.hpp" // for __minimk_socket_create

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for C function declarations

minimk_error_t minimk_socket_init(void) noexcept {
    return 0;
}

int minimk_socket_af_inet(void) noexcept {
    return AF_INET;
}

int minimk_socket_af_inet6(void) noexcept {
    return AF_INET6;
}

int minimk_socket_sock_stream(void) noexcept {
    return SOCK_STREAM;
}

int minimk_socket_sock_dgram(void) noexcept {
    return SOCK_DGRAM;
}

minimk_socket_t minimk_socket_invalid(void) noexcept {
    return -1;
}

minimk_error_t minimk_socket_create(minimk_socket_t *sock, int domain, int type, int protocol) noexcept {
    return __minimk_socket_create(sock, domain, type, protocol);
}

minimk_error_t minimk_socket_setnonblock(minimk_socket_t sock) noexcept {
    return __minimk_socket_setnonblock(sock);
}

minimk_error_t minimk_socket_connect(minimk_socket_t sock, const char *address, const char *port) noexcept {
    return __minimk_socket_connect(sock, address, port);
}

minimk_error_t minimk_socket_bind(minimk_socket_t sock, const char *address, const char *port) noexcept {
    return __minimk_socket_bind(sock, address, port);
}

minimk_error_t minimk_socket_listen(minimk_socket_t sock, int backlog) noexcept {
    return __minimk_socket_listen(sock, backlog);
}

minimk_error_t minimk_socket_accept(minimk_socket_t *client_sock, minimk_socket_t sock) noexcept {
    return __minimk_socket_accept(client_sock, sock);
}

minimk_error_t minimk_socket_recv(minimk_socket_t sock, void *data, size_t count, size_t *nread) noexcept {
    return __minimk_socket_recv(sock, data, count, nread);
}

minimk_error_t minimk_socket_send(minimk_socket_t sock, const void *data, size_t count, size_t *nwritten) noexcept {
    return __minimk_socket_send(sock, data, count, nwritten);
}

minimk_error_t minimk_socket_destroy(minimk_socket_t *sock) noexcept {
    return __minimk_socket_destroy(sock);
}

minimk_error_t minimk_socket_sendall(minimk_socket_t sock, const void *buf, size_t count) noexcept {
    return __minimk_io_writeall<minimk_socket_t, minimk_socket_send>(sock, buf, count);
}

minimk_error_t minimk_socket_recvall(minimk_socket_t sock, void *buf, size_t count) noexcept {
    return __minimk_io_readall<minimk_socket_t, minimk_socket_recv>(sock, buf, count);
}
