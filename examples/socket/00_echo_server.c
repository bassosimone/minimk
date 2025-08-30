// File: examples/socker/00_echo_server.c
// Purpose: nonblocking TCP echo server using the runtime
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/errno.h>   // for minimk_errno_name
#include <minimk/runtime.h> // for minimk_runtime_go
#include <minimk/socket.h>  // for minimk_socket_*
#include <minimk/syscall.h> // for minimk_syscall_socket_init
#include <minimk/trace.h>   // for minimk_trace_enable

#include <stdio.h>  // for fprintf
#include <stdlib.h> // for exit

/// Takes ownweship of the socket and echoes back data to the client.
static void handle_client(void *opaque) {
    // Log that we have a connection and take socket ownership.
    fprintf(stderr, "Client connected\n");
    minimk_socket_t client_sock = (minimk_socket_t)opaque;

    char buffer[4096];
    for (;;) {
        // Read some bytes from the client
        size_t nread = 0;
        minimk_error_t rv = minimk_socket_recv(client_sock, buffer, sizeof(buffer), &nread);

        // Handle the potential errors
        if (rv == MINIMK_EOF) {
            fprintf(stderr, "Client disconnected\n");
            break;
        }
        if (rv != 0) {
            fprintf(stderr, "Read error: %s\n", minimk_errno_name(rv));
            break;
        }
        MINIMK_ASSERT(nread > 0);

        // Echo back the data
        rv = minimk_socket_sendall(client_sock, buffer, nread);

        // Handle potential errors
        if (rv != 0) {
            fprintf(stderr, "Write error: %s\n", minimk_errno_name(rv));
            break;
        }
    }

    fprintf(stderr, "Closing the client socket\n");
    minimk_socket_destroy(&client_sock);
}

/// Borrow the server socket and accept connections.
static void accept_loop(void *opaque) __attribute__((noreturn));

static void accept_loop(void *opaque) {
    minimk_socket_t ssock = (minimk_socket_t)opaque;

    for (;;) {
        // Attempt to accept an incoming conn
        minimk_socket_t client_sock = MINIMK_SOCKET_INVALID;
        minimk_error_t rv = minimk_socket_accept(&client_sock, ssock);

        // Handle the case of failure
        if (rv != 0) {
            fprintf(stderr, "Socket accept failed: %s\n", minimk_errno_name(rv));
            continue;
        }

        // Start the coroutine and pass socket ownership
        minimk_runtime_go(handle_client, (void *)client_sock);
    }
}

int main(void) {
    // Configure tracing to observe events
    minimk_trace_enable |= MINIMK_TRACE_ENABLE_SYSCALL;

    // Initialize socket library
    minimk_error_t rv = minimk_syscall_socket_init();
    if (rv != 0) {
        fprintf(stderr, "Socket init failed: %s\n", minimk_errno_name(rv));
        exit(1);
    }

    // Create listening socket
    minimk_socket_t server_sock = MINIMK_SOCKET_INVALID;
    rv = minimk_socket_create(&server_sock, minimk_syscall_af_inet, minimk_syscall_sock_stream, 0);
    if (rv != 0) {
        fprintf(stderr, "Socket create failed: %s\n", minimk_errno_name(rv));
        exit(1);
    }

    // Bind to localhost:9774
    rv = minimk_socket_bind(server_sock, "127.0.0.1", "9774");
    if (rv != 0) {
        fprintf(stderr, "Socket bind failed: %s\n", minimk_errno_name(rv));
        minimk_socket_destroy(&server_sock);
        exit(1);
    }

    // Start listening
    rv = minimk_socket_listen(server_sock, 128);
    if (rv != 0) {
        fprintf(stderr, "Socket listen failed: %s\n", minimk_errno_name(rv));
        minimk_socket_destroy(&server_sock);
        exit(1);
    }

    fprintf(stderr, "Echo server listening on 127.0.0.1:9774\n");
    fprintf(stderr, "Test with: nc 127.0.0.1 9774\n");

    // Spawn the goroutine that accepts connections
    minimk_runtime_go(accept_loop, (void *)server_sock);

    // Block on running the goroutines.
    minimk_runtime_run();

    // Cleanup the server socket and exit.
    minimk_socket_destroy(&server_sock);
    return 0;
}
