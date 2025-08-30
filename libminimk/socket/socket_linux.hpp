// File: libminimk/socket/socket_linux.hpp
// Purpose: socket library implemented for linux
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_SOCKET_SOCKET_LINUX_HPP
#define LIBMINIMK_SOCKET_SOCKET_LINUX_HPP

#include "../runtime/trace.h" // for MINIMK_TRACE
#include "socket.h"           // for minimk_socket_t

#include <minimk/errno.h> // for minimk_error_t

#include <sys/socket.h> // for recv
#include <sys/types.h>  // for ssize_t

#include <fcntl.h>  // for fcntl
#include <limits.h> // for SSIZE_MAX
#include <netdb.h>  // for getaddrinfo
#include <stddef.h> // for size_t
#include <unistd.h> // for close

#endif // LIBMINIMK_SOCKET_SOCKET_LINUX_HPP
