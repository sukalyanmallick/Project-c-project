/*
 * server/src/network_server.c — Server Network Layer Implementation
 *
 * Implements the four-phase Winsock2 TCP server lifecycle. Each function
 * handles exactly one phase; see network_server.h for the usage pattern.
 *
 * Error handling convention:
 *   All socket errors print the Winsock error code to stderr so failures
 *   can be diagnosed without an attached debugger.
 */

#include "../include/network_server.h"
#include "../include/ai_engine.h"
#include "../../shared/constants.h"

#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

/* ════════════════════════════════════════════════════════════════════════════
 * Internal helpers
 * ════════════════════════════════════════════════════════════════════════════ */

/*
 * BuildServerAddress — Populate a sockaddr_in for the listening socket.
 * Binds to all interfaces (INADDR_ANY) on DEFAULT_SERVER_PORT.
 */
static struct sockaddr_in BuildServerAddress(void) {
    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(DEFAULT_SERVER_PORT);
    return addr;
}

/*
 * SessionShouldExit — Return non-zero if the session loop should end.
 * Currently: the client explicitly typed "bye".
 */
static int SessionShouldExit(const char *message) {
    return strstr(message, "bye") != NULL;
}

/* ════════════════════════════════════════════════════════════════════════════
 * Phase 1 — Server_Initialize
 * ════════════════════════════════════════════════════════════════════════════ */

int Server_Initialize(ServerContext *ctx) {
    ctx->serverSocket = INVALID_SOCKET;
    ctx->clientSocket = INVALID_SOCKET;

    /* Start Winsock 2.2 */
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "[Server] WSAStartup failed. Error: %d\n",
                WSAGetLastError());
        return 0;
    }

    /* Create the TCP listening socket */
    ctx->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ctx->serverSocket == INVALID_SOCKET) {
        fprintf(stderr, "[Server] socket() failed. Error: %d\n",
                WSAGetLastError());
        WSACleanup();
        return 0;
    }

    struct sockaddr_in serverAddr = BuildServerAddress();

    if (bind(ctx->serverSocket,
             (struct sockaddr *)&serverAddr,
             sizeof(serverAddr)) == SOCKET_ERROR) {
        fprintf(stderr, "[Server] bind() failed. Error: %d\n",
                WSAGetLastError());
        closesocket(ctx->serverSocket);
        WSACleanup();
        return 0;
    }

    if (listen(ctx->serverSocket, SERVER_LISTEN_BACKLOG) == SOCKET_ERROR) {
        fprintf(stderr, "[Server] listen() failed. Error: %d\n",
                WSAGetLastError());
        closesocket(ctx->serverSocket);
        WSACleanup();
        return 0;
    }

    printf("[Server] Listening on port %d...\n", DEFAULT_SERVER_PORT);
    return 1;
}

/* ════════════════════════════════════════════════════════════════════════════
 * Phase 2 — Server_WaitForClient
 * ════════════════════════════════════════════════════════════════════════════ */

int Server_WaitForClient(ServerContext *ctx) {
    printf("[Server] Waiting for a client...\n");

    struct sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);

    ctx->clientSocket = accept(ctx->serverSocket,
                               (struct sockaddr *)&clientAddr,
                               &addrLen);

    if (ctx->clientSocket == INVALID_SOCKET) {
        fprintf(stderr, "[Server] accept() failed. Error: %d\n",
                WSAGetLastError());
        return 0;
    }

    printf("[Server] Client connected.\n");
    return 1;
}

/* ════════════════════════════════════════════════════════════════════════════
 * Phase 3 — Server_HandleClientSession
 * ════════════════════════════════════════════════════════════════════════════ */

void Server_HandleClientSession(ServerContext *ctx) {
    char incomingMessage[SERVER_BUFFER_SIZE];
    char outgoingReply[SERVER_BUFFER_SIZE];

    while (1) {
        memset(incomingMessage, 0, sizeof(incomingMessage));

        int bytesReceived = recv(ctx->clientSocket,
                                 incomingMessage,
                                 SERVER_BUFFER_SIZE - 1,
                                 0);

        /* Clean disconnect (0) or network error (< 0) */
        if (bytesReceived <= 0) {
            printf("[Server] Client disconnected.\n");
            break;
        }

        incomingMessage[bytesReceived] = '\0';
        printf("[Server] Client: %s\n", incomingMessage);

        /* Generate the AI reply and copy it safely into the send buffer */
        const char *aiResponse = AI_GenerateReply(incomingMessage);
        strncpy(outgoingReply, aiResponse, SERVER_BUFFER_SIZE - 1);
        outgoingReply[SERVER_BUFFER_SIZE - 1] = '\0';

        if (send(ctx->clientSocket,
                 outgoingReply,
                 (int)strlen(outgoingReply),
                 0) == SOCKET_ERROR) {
            fprintf(stderr, "[Server] send() failed. Error: %d\n",
                    WSAGetLastError());
            break;
        }

        if (SessionShouldExit(incomingMessage)) {
            printf("[Server] Session ended by client.\n");
            break;
        }
    }
}

/* ════════════════════════════════════════════════════════════════════════════
 * Phase 4 — Server_Shutdown
 * ════════════════════════════════════════════════════════════════════════════ */

void Server_Shutdown(ServerContext *ctx) {
    if (ctx->clientSocket != INVALID_SOCKET) {
        closesocket(ctx->clientSocket);
        ctx->clientSocket = INVALID_SOCKET;
    }

    if (ctx->serverSocket != INVALID_SOCKET) {
        closesocket(ctx->serverSocket);
        ctx->serverSocket = INVALID_SOCKET;
    }

    WSACleanup();
    printf("[Server] Shutdown complete.\n");
}
