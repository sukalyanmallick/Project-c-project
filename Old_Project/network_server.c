/*
 * network_server.c — Server Network Layer Implementation
 *
 * Implements the Winsock2 TCP server lifecycle broken into discrete,
 * single-responsibility functions. Each function corresponds to one
 * phase of the server's operation.
 *
 * All socket errors are reported to stderr with the Winsock error code
 * so they can be diagnosed without a debugger attached.
 */

#include "network_server.h"
#include "ai_engine.h"
#include "../shared/constants.h"

#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

/* ─── Server_Initialize ──────────────────────────────────────────────────── */

int Server_Initialize(ServerContext *ctx) {
    /*
     * Zero-out the context so INVALID_SOCKET checks in Server_Shutdown()
     * are safe even if this function returns early.
     */
    ctx->serverSocket = INVALID_SOCKET;
    ctx->clientSocket = INVALID_SOCKET;

    /* Start Winsock version 2.2 */
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed. Error: %d\n", WSAGetLastError());
        return 0;
    }

    /* Create the TCP listening socket */
    ctx->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ctx->serverSocket == INVALID_SOCKET) {
        fprintf(stderr, "Socket creation failed. Error: %d\n", WSAGetLastError());
        WSACleanup();
        return 0;
    }

    /* Configure the server address: listen on all interfaces at DEFAULT_SERVER_PORT */
    struct sockaddr_in serverAddress;
    serverAddress.sin_family      = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port        = htons(DEFAULT_SERVER_PORT);

    if (bind(ctx->serverSocket,
             (struct sockaddr *)&serverAddress,
             sizeof(serverAddress)) == SOCKET_ERROR) {
        fprintf(stderr, "Bind failed. Error: %d\n", WSAGetLastError());
        closesocket(ctx->serverSocket);
        WSACleanup();
        return 0;
    }

    if (listen(ctx->serverSocket, SERVER_LISTEN_BACKLOG) == SOCKET_ERROR) {
        fprintf(stderr, "Listen failed. Error: %d\n", WSAGetLastError());
        closesocket(ctx->serverSocket);
        WSACleanup();
        return 0;
    }

    printf("AI Chatbot Server started. Listening on port %d...\n",
           DEFAULT_SERVER_PORT);
    return 1;
}

/* ─── Server_WaitForClient ───────────────────────────────────────────────── */

int Server_WaitForClient(ServerContext *ctx) {
    printf("Waiting for a client to connect...\n");

    struct sockaddr_in clientAddress;
    int clientAddressLength = sizeof(clientAddress);

    /*
     * accept() blocks here until a client connects.
     * On success, clientSocket is a new socket dedicated to that client.
     */
    ctx->clientSocket = accept(ctx->serverSocket,
                               (struct sockaddr *)&clientAddress,
                               &clientAddressLength);

    if (ctx->clientSocket == INVALID_SOCKET) {
        fprintf(stderr, "accept() failed. Error: %d\n", WSAGetLastError());
        return 0;
    }

    printf("Client connected!\n");
    return 1;
}

/* ─── Server_HandleClientSession ─────────────────────────────────────────── */

void Server_HandleClientSession(ServerContext *ctx) {
    char incomingMessage[SERVER_BUFFER_SIZE];
    char outgoingReply[SERVER_BUFFER_SIZE];

    /*
     * Process messages in a loop until the client disconnects or sends "bye".
     * recv() returns 0 on clean disconnect, SOCKET_ERROR on network failure.
     */
    while (1) {
        memset(incomingMessage, 0, sizeof(incomingMessage));

        /*
         * Block until data is received from the client.
         * Limit read to SERVER_BUFFER_SIZE - 1 to leave room for null terminator.
         */
        int bytesReceived = recv(ctx->clientSocket,
                                 incomingMessage,
                                 SERVER_BUFFER_SIZE - 1,
                                 0);

        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            printf("Client disconnected.\n");
            break;
        }

        incomingMessage[bytesReceived] = '\0';
        printf("Client: %s\n", incomingMessage);

        /* Generate a reply using the AI strategy engine */
        const char *aiResponse = AI_GenerateReply(incomingMessage);

        /*
         * Use strncpy instead of strcpy to prevent buffer overrun.
         * Explicitly null-terminate in case the source is exactly SERVER_BUFFER_SIZE - 1.
         */
        strncpy(outgoingReply, aiResponse, SERVER_BUFFER_SIZE - 1);
        outgoingReply[SERVER_BUFFER_SIZE - 1] = '\0';

        if (send(ctx->clientSocket,
                 outgoingReply,
                 (int)strlen(outgoingReply),
                 0) == SOCKET_ERROR) {
            fprintf(stderr, "send() failed. Error: %d\n", WSAGetLastError());
            break;
        }

        /* "bye" from the client signals a polite disconnect */
        if (strstr(incomingMessage, "bye") != NULL) {
            printf("Client sent 'bye'. Closing session.\n");
            break;
        }
    }
}

/* ─── Server_Shutdown ────────────────────────────────────────────────────── */

void Server_Shutdown(ServerContext *ctx) {
    /*
     * Close sockets only if they were successfully opened.
     * INVALID_SOCKET checks guard against double-close if initialization
     * failed partway through Server_Initialize().
     */
    if (ctx->clientSocket != INVALID_SOCKET) {
        closesocket(ctx->clientSocket);
        ctx->clientSocket = INVALID_SOCKET;
    }

    if (ctx->serverSocket != INVALID_SOCKET) {
        closesocket(ctx->serverSocket);
        ctx->serverSocket = INVALID_SOCKET;
    }

    WSACleanup();
    printf("Server shut down cleanly.\n");
}
