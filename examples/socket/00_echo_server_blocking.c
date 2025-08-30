// File: examples/socket/00_echo_server_blocking.c
// Purpose: blocking TCP echo server using socket API
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/assert.h> // for MINIMK_ASSERT
#include <minimk/errno.h>  // for minimk_errno_name
#include <minimk/socket.h> // for minimk_socket_init + constants
#include "../../libminimk/socket/socket.h" // for minimk_socket_* operations

#include <stdio.h>  // for fprintf
#include <stdlib.h> // for exit

/// Takes ownweship of the socket and echoes back data to the client.
static void handle_client(minimk_socket_t client_sock) {
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

    minimk_socket_destroy(&client_sock);
}

int main(void) {
    // Initialize socket library
    minimk_error_t rv = minimk_socket_init();
    if (rv != 0) {
        fprintf(stderr, "Socket init failed: %s\n", minimk_errno_name(rv));
        exit(1);
    }

    // Create listening socket
    minimk_socket_t server_sock = minimk_socket_invalid();
    rv = minimk_socket_create(&server_sock, minimk_socket_af_inet(), minimk_socket_sock_stream(), 0);
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

    // Accept connections
    for (;;) {
        minimk_socket_t client_sock = minimk_socket_invalid();
        rv = minimk_socket_accept(&client_sock, server_sock);

        if (rv != 0) {
            fprintf(stderr, "Socket accept failed: %s\n", minimk_errno_name(rv));
            continue;
        }

        fprintf(stderr, "Client connected\n");
        handle_client(client_sock);
    }

    minimk_socket_destroy(&server_sock);
    return 0;
}
