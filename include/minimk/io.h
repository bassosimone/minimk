// File: include/minimk/io.h
// Purpose: I/O library
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MINIMK_IO_H
#define MINIMK_IO_H

#include <minimk/core.h> // for MINIMK_BEGIN_DECLS

#include <minimk/errno.h>  // for minimk_error_t
#include <minimk/socket.h> // for minimk_socket_send

#include <stddef.h> // for size_t

MINIMK_BEGIN_DECLS

/// Instantiation of `minimk_io_writeall` for `minimk_socket_send`.
minimk_error_t
minimk_io_socket_writeall(minimk_socket_t sock, const void *buf, size_t count, size_t *total) MINIMK_NOEXCEPT;

/// Instantiation of `minimk_io_readall` for `minimk_socket_recv`.
minimk_error_t minimk_io_socket_readall(minimk_socket_t sock, void *buf, size_t count, size_t *total) MINIMK_NOEXCEPT;

MINIMK_END_DECLS

#endif // MINIMK_IO_H
