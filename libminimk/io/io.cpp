// File: libminimk/io/io.cpp
// Purpose: I/O library
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/io.hpp> // for minimk_io_writeall

// Explicit template instantiations for known functions
template minimk_error_t
minimk_io_writeall<minimk_socket_send>(minimk_socket_t, const void *, size_t, size_t *) noexcept;

template minimk_error_t minimk_io_readall<minimk_socket_recv>(minimk_socket_t, void *, size_t, size_t *) noexcept;
