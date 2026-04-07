/*
 * network_client.c — Client Network Layer Implementation
 *
 * Implements the Winsock2 TCP connection lifecycle for the GTK4 chat client.
 * All network state is stored in NetworkContext (embedded in ChatApplicationState)
 * — no global variables are used.
 *
 * Thread safety:
 *   NetworkContext.isConnected is a gint accessed exclusively via
 *   g_atomic_int_get() and g_atomic_int_set() to prevent races between
 *   the GTK main thread and the background receive thread.
 */

#include "network_client.h"
#include "ui.h"
#include "../shared/constants.h"

#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

/* ─── Network_Connect ────────────────────────────────────────────────────── */

void Network_Connect(ChatApplicationState *appState) {
    NetworkContext *ctx = &appState->network;

    /*
     * Guard against double-connect. The isConnected flag is read atomically
     * because the receive thread may be writing to it concurrently.
     */
    if (g_atomic_int_get(&ctx->isConnected)) {
        UI_AppendMessage(appState, "Already connected to the server.");
        return;
    }

    /* Start Winsock version 2.2 */
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        UI_AppendMessage(appState, "WSAStartup failed. Cannot connect.");
        return;
    }

    /* Create a TCP socket */
    ctx->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ctx->socket == INVALID_SOCKET) {
        UI_AppendMessage(appState, "Socket creation failed.");
        WSACleanup();
        return;
    }

    /* Build the server address from compile-time defaults */
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port   = htons(DEFAULT_SERVER_PORT);
    serverAddress.sin_addr.s_addr = inet_addr(DEFAULT_SERVER_IP);

    if (serverAddress.sin_addr.s_addr == INADDR_NONE) {
        UI_AppendMessage(appState, "Invalid server IP address in constants.h.");
        closesocket(ctx->socket);
        WSACleanup();
        return;
    }

    UI_AppendMessage(appState, "Connecting to server...");

    int connectResult = connect(ctx->socket,
                                (struct sockaddr *)&serverAddress,
                                sizeof(serverAddress));
    if (connectResult == SOCKET_ERROR) {
        UI_AppendMessage(appState,
            "Connection failed. Is the server running?");
        closesocket(ctx->socket);
        WSACleanup();
        return;
    }

    /*
     * Mark connected atomically before spawning the thread so the thread
     * sees isConnected = 1 immediately when it starts.
     */
    g_atomic_int_set(&ctx->isConnected, 1);
    UI_AppendMessage(appState, "Connected to server.");

    /*
     * Spawn the background receive thread, passing the full appState so
     * the thread can post messages to the UI via g_idle_add().
     */
    ctx->receiveThread = g_thread_new("receive_thread",
                                      Network_ReceiveLoop,
                                      appState);
}

/* ─── Network_SendMessage ────────────────────────────────────────────────── */

int Network_SendMessage(NetworkContext *ctx, const char *message) {
    g_assert(message != NULL);

    if (!g_atomic_int_get(&ctx->isConnected)) {
        return 0;
    }

    /*
     * Validate message length before sending.
     * Messages longer than CLIENT_BUFFER_SIZE - 1 are silently truncated
     * by the send() call; log a warning if this happens.
     */
    size_t messageLength = strlen(message);
    if (messageLength == 0) {
        return 0;
    }

    if (messageLength >= CLIENT_BUFFER_SIZE) {
        fprintf(stderr,
            "Warning: message truncated to %d bytes before send.\n",
            CLIENT_BUFFER_SIZE - 1);
        messageLength = CLIENT_BUFFER_SIZE - 1;
    }

    if (send(ctx->socket, message, (int)messageLength, 0) == SOCKET_ERROR) {
        fprintf(stderr, "send() failed. Error: %d\n", WSAGetLastError());
        return 0;
    }

    return 1;
}

/* ─── Network_ReceiveLoop ────────────────────────────────────────────────── */

gpointer Network_ReceiveLoop(gpointer data) {
    /*
     * This function runs on a background thread. It must never call GTK
     * functions directly — all UI updates go through g_idle_add() so
     * they execute safely on the GTK main thread.
     */
    ChatApplicationState *appState = (ChatApplicationState *)data;
    NetworkContext       *ctx      = &appState->network;

    char receiveBuffer[CLIENT_BUFFER_SIZE];

    while (g_atomic_int_get(&ctx->isConnected)) {
        memset(receiveBuffer, 0, sizeof(receiveBuffer));

        int bytesReceived = recv(ctx->socket,
                                 receiveBuffer,
                                 CLIENT_BUFFER_SIZE - 1,
                                 0);

        if (bytesReceived <= 0) {
            /*
             * bytesReceived == 0  → server closed the connection cleanly.
             * bytesReceived < 0   → network error.
             */
            break;
        }

        receiveBuffer[bytesReceived] = '\0';

        /*
         * Heap-allocate the payload for the idle callback.
         * UI_AppendMessage_Idle() takes ownership and frees both the
         * IdleMessageData and its message field.
         */
        IdleMessageData *payload = g_new(IdleMessageData, 1);
        payload->appState = appState;
        payload->message  = g_strdup_printf("Bot: %s", receiveBuffer);

        g_idle_add(UI_AppendMessage_Idle, payload);
    }

    /*
     * Connection lost (either cleanly or by error). Post a disconnection
     * notice to the UI and mark the context as disconnected.
     */
    g_atomic_int_set(&ctx->isConnected, 0);

    IdleMessageData *disconnectPayload = g_new(IdleMessageData, 1);
    disconnectPayload->appState = appState;
    disconnectPayload->message  = g_strdup("Disconnected from server.");
    g_idle_add(UI_AppendMessage_Idle, disconnectPayload);

    return NULL;
}

/* ─── Network_Disconnect ─────────────────────────────────────────────────── */

void Network_Disconnect(NetworkContext *ctx) {
    /*
     * Set isConnected = 0 atomically first so the receive thread exits
     * its loop naturally on the next iteration.
     */
    g_atomic_int_set(&ctx->isConnected, 0);

    if (ctx->socket != INVALID_SOCKET) {
        closesocket(ctx->socket);
        ctx->socket = INVALID_SOCKET;
    }

    WSACleanup();
}
