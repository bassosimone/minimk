// File: examples/socket/01_echo_test.c
// Purpose: integrated server+client test using cooperative coroutines
// SPDX-License-Identifier: GPL-3.0-or-later

#include <minimk/assert.h>  // for MINIMK_ASSERT
#include <minimk/errno.h>   // for minimk_errno_name
#include <minimk/runtime.h> // for minimk_runtime_go
#include <minimk/socket.h>  // for minimk_socket_*
#include <minimk/syscall.h> // for minimk_syscall_socket_init
#include <minimk/trace.h>   // for minimk_trace_enable

#include <stdio.h>  // for fprintf
#include <stdlib.h> // for exit
#include <string.h> // for strlen, memcmp

/// Test data to send from client to server.
static const char *test_messages[] = {"Hello, World!",      "Testing echo functionality",
                                      "1234567890",         "Special chars: !@#$%^&*()",
                                      "Final test message", NULL};

/// Global variables for test results.
static int test_passed = 0;

/// Client coroutine that connects and sends test data.
static void echo_client(void *opaque) {
    (void)opaque;

    fprintf(stderr, "Client: Connecting to server\n");

    // Create client socket
    minimk_socket_t sock = MINIMK_SOCKET_INVALID;
    minimk_error_t rv = minimk_socket_create(&sock, minimk_syscall_af_inet, minimk_syscall_sock_stream, 0);
    if (rv != 0) {
        fprintf(stderr, "Client: Socket create failed: %s\n", minimk_errno_name(rv));
        return;
    }

    // Connect to server
    rv = minimk_socket_connect(sock, "127.0.0.1", "12345");
    if (rv != 0) {
        fprintf(stderr, "Client: Connect failed: %s\n", minimk_errno_name(rv));
        minimk_socket_destroy(&sock);
        return;
    }

    fprintf(stderr, "Client: Connected successfully\n");

    // Send each test message and verify echo
    int all_tests_passed = 1;
    for (size_t i = 0; i < 5; i++) {
        // We know there are 5 test messages
        const char *msg = test_messages[i];
        size_t msg_len = strlen(msg);

        fprintf(stderr, "Client: Sending message %zu: '%s'\n", i + 1, msg);

        // Send the message
        rv = minimk_socket_sendall(sock, msg, msg_len);
        if (rv != 0) {
            fprintf(stderr, "Client: Send failed: %s\n", minimk_errno_name(rv));
            all_tests_passed = 0;
            break;
        }

        // Receive the echo
        char buffer[1024];
        rv = minimk_socket_recvall(sock, buffer, msg_len);
        if (rv != 0) {
            fprintf(stderr, "Client: Recv failed: %s\n", minimk_errno_name(rv));
            all_tests_passed = 0;
            break;
        }

        // Verify the echo matches
        if (memcmp(buffer, msg, msg_len) != 0) {
            fprintf(stderr, "Client: Echo mismatch for message %zu\n", i + 1);
            all_tests_passed = 0;
            break;
        }

        fprintf(stderr, "Client: Message %zu echoed correctly\n", i + 1);
    }

    // Close client connection
    minimk_socket_destroy(&sock);

    if (all_tests_passed) {
        fprintf(stderr, "Client: All tests PASSED!\n");
        test_passed = 1;
    } else {
        fprintf(stderr, "Client: Some tests FAILED!\n");
    }
}

/// Server coroutine that accepts one connection and handles echo.
static void echo_server(void *opaque) {
    minimk_socket_t server_sock = (minimk_socket_t)opaque;

    fprintf(stderr, "Server: Ready to accept connections\n");

    // Start the client coroutine now that server is listening
    minimk_runtime_go(echo_client, NULL);

    // Accept exactly one connection
    minimk_socket_t client_sock = MINIMK_SOCKET_INVALID;
    minimk_error_t rv = minimk_socket_accept(&client_sock, server_sock);

    if (rv != 0) {
        fprintf(stderr, "Server: Accept failed: %s\n", minimk_errno_name(rv));
        return;
    }

    fprintf(stderr, "Server: Client connected\n");

    char buffer[1024];
    for (;;) {
        // Read data from client
        size_t nread = 0;
        rv = minimk_socket_recv(client_sock, buffer, sizeof(buffer), &nread);

        if (rv == MINIMK_EOF) {
            fprintf(stderr, "Server: Client disconnected\n");
            break;
        }

        if (rv != 0) {
            fprintf(stderr, "Server: Read error: %s\n", minimk_errno_name(rv));
            break;
        }

        MINIMK_ASSERT(nread > 0);

        // Echo the data back
        rv = minimk_socket_sendall(client_sock, buffer, nread);
        if (rv != 0) {
            fprintf(stderr, "Server: Write error: %s\n", minimk_errno_name(rv));
            break;
        }

        fprintf(stderr, "Server: Echoed %zu bytes\n", nread);
    }

    minimk_socket_destroy(&client_sock);
}

int main(void) {
    // Configure minimal tracing
    minimk_trace_enable |= MINIMK_TRACE_ENABLE_SOCKET;

    // Initialize socket library
    minimk_error_t rv = minimk_syscall_socket_init();
    if (rv != 0) {
        fprintf(stderr, "Socket init failed: %s\n", minimk_errno_name(rv));
        exit(1);
    }

    fprintf(stderr, "Starting integrated server+client echo test\n");

    // Create and configure server socket
    static minimk_socket_t server_sock = MINIMK_SOCKET_INVALID;
    rv = minimk_socket_create(&server_sock, minimk_syscall_af_inet, minimk_syscall_sock_stream, 0);
    if (rv != 0) {
        fprintf(stderr, "Server socket create failed: %s\n", minimk_errno_name(rv));
        exit(1);
    }

    rv = minimk_socket_setsockopt_reuseaddr(server_sock);
    if (rv != 0) {
        fprintf(stderr, "Server setsockopt failed: %s\n", minimk_errno_name(rv));
        minimk_socket_destroy(&server_sock);
        exit(1);
    }

    rv = minimk_socket_bind(server_sock, "127.0.0.1", "12345");
    if (rv != 0) {
        fprintf(stderr, "Server bind failed: %s\n", minimk_errno_name(rv));
        minimk_socket_destroy(&server_sock);
        exit(1);
    }

    rv = minimk_socket_listen(server_sock, 1);
    if (rv != 0) {
        fprintf(stderr, "Server listen failed: %s\n", minimk_errno_name(rv));
        minimk_socket_destroy(&server_sock);
        exit(1);
    }

    fprintf(stderr, "Server listening on 127.0.0.1:12345\n");

    // Start only the server coroutine - it will start the client when ready
    minimk_runtime_go(echo_server, (void *)server_sock);

    // Run the event loop
    minimk_runtime_run();

    // Cleanup
    minimk_socket_destroy(&server_sock);

    if (test_passed) {
        fprintf(stderr, "\n=== ECHO TEST PASSED ===\n");
        return 0;
    } else {
        fprintf(stderr, "\n=== ECHO TEST FAILED ===\n");
        return 1;
    }
}
