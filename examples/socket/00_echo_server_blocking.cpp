// File: examples/socket/00_echo_server_blocking.cpp
// Purpose: blocking TCP echo server using socket API
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/errno.h>   // for minimk_errno_name
#include <minimk/io.hpp>    // for minimk_io_writeall
#include <minimk/syscall.h> // for minimk_syscall_socket_init
#include <minimk/trace.h>   // for minimk_trace_enable

#include <cstdlib>  // for exit
#include <iostream> // for std::cerr

/// Takes ownweship of the socket and echoes back data to the client.
static void handle_client(minimk_syscall_socket_t client_sock) {
    char buffer[4096];

    for (;;) {
        // Read some bytes from the client
        size_t nread = 0;
        minimk_error_t rv = minimk_syscall_recv(client_sock, buffer, sizeof(buffer), &nread);

        // Handle the potential errors
        if (rv == MINIMK_EOF) {
            std::cerr << "Client disconnected" << std::endl;
            break;
        }
        if (rv != 0) {
            std::cerr << "Read error: " << minimk_errno_name(rv) << std::endl;
            break;
        }
        MINIMK_ASSERT(nread > 0);

        // Echo back the data
        rv = minimk_io_writeall<minimk_syscall_socket_t, minimk_syscall_send>(client_sock, buffer,
                                                                              nread);

        // Handle potential errors
        if (rv != 0) {
            std::cerr << "Write error: " << minimk_errno_name(rv) << std::endl;
            break;
        }
    }

    minimk_syscall_closesocket(&client_sock);
}

int main(void) {
    // Configure tracing at the syscall level
    minimk_trace_enable |= MINIMK_TRACE_ENABLE_SYSCALL;

    // Initialize socket library
    minimk_error_t rv = minimk_syscall_socket_init();
    if (rv != 0) {
        std::cerr << "Socket init failed: " << minimk_errno_name(rv) << std::endl;
        exit(1);
    }

    // Create listening socket
    minimk_syscall_socket_t server_sock = minimk_syscall_invalid_socket;
    rv = minimk_syscall_socket(&server_sock, minimk_syscall_af_inet, minimk_syscall_sock_stream, 0);
    if (rv != 0) {
        std::cerr << "Socket create failed: " << minimk_errno_name(rv) << std::endl;
        exit(1);
    }

    // Bind to localhost:9774
    rv = minimk_syscall_bind(server_sock, "127.0.0.1", "9774");
    if (rv != 0) {
        std::cerr << "Socket bind failed: " << minimk_errno_name(rv) << std::endl;
        minimk_syscall_closesocket(&server_sock);
        exit(1);
    }

    // Start listening
    rv = minimk_syscall_listen(server_sock, 128);
    if (rv != 0) {
        std::cerr << "Socket listen failed: " << minimk_errno_name(rv) << std::endl;
        minimk_syscall_closesocket(&server_sock);
        exit(1);
    }

    std::cerr << "Echo server listening on 127.0.0.1:9774" << std::endl;
    std::cerr << "Test with: nc 127.0.0.1 9774" << std::endl;

    // Accept connections
    for (;;) {
        minimk_syscall_socket_t client_sock = minimk_syscall_invalid_socket;
        rv = minimk_syscall_accept(&client_sock, server_sock);

        if (rv != 0) {
            std::cerr << "Socket accept failed: " << minimk_errno_name(rv) << std::endl;
            break;
        }

        std::cerr << "Client connected" << std::endl;
        handle_client(client_sock);
    }

    minimk_syscall_closesocket(&server_sock);
    return 0;
}
