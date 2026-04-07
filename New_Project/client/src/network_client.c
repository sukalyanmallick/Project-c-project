/*
 * client/src/network_client.c — Client Network Layer Implementation
 *
 * Implements the Winsock2 TCP connection lifecycle for the GTK4 chat
 * client. All network state lives in NetworkContext (embedded in
 * ChatApplicationState) — no global variables are used.
 *
 * Thread model summary:
 *   Main thread  — Connect / Send / Disconnect
 *   Recv thread  — recv() loop → g_idle_add() → GTK main thread
 */

#include "../include/network_client.h"
#include "../include/ui.h"
#include "../../shared/constants.h"

#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

/* ════════════════════════════════════════════════════════════════════════════
 * Internal helpers
 * ════════════════════════════════════════════════════════════════════════════ */

/*
 * BuildServerAddress — Return a sockaddr_in for DEFAULT_SERVER_IP:PORT.
 * Returns an address with sin_addr.s_addr == INADDR_NONE on bad IP.
 */
static struct sockaddr_in BuildServerAddress(void) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(DEFAULT_SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(DEFAULT_SERVER_IP);
    return addr;
}

/*
 * PostMessageToUI — Heap-allocate an IdleMessageData and schedule it on
 * the GTK main thread. Ownership of the payload transfers to the callback.
 */
static void PostMessageToUI(ChatApplicationState *appState, const char *text) {
    IdleMessageData *payload = g_new(IdleMessageData, 1);
    payload->appState = appState;
    payload->message  = g_strdup(text);
    g_idle_add(UI_AppendMessage_Idle, payload);
}

/* ════════════════════════════════════════════════════════════════════════════
 * Network_Connect
 * ════════════════════════════════════════════════════════════════════════════ */

void Network_Connect(ChatApplicationState *appState) {
    NetworkContext *ctx = &appState->network;

    if (g_atomic_int_get(&ctx->isConnected)) {
        UI_AppendMessage(appState, "Already connected to the server.");
        return;
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        UI_AppendMessage(appState, "WSAStartup failed. Cannot connect.");
        return;
    }

    ctx->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ctx->socket == INVALID_SOCKET) {
        UI_AppendMessage(appState, "Socket creation failed.");
        WSACleanup();
        return;
    }

    struct sockaddr_in serverAddr = BuildServerAddress();

    if (serverAddr.sin_addr.s_addr == INADDR_NONE) {
        UI_AppendMessage(appState, "Invalid server IP in constants.h.");
        closesocket(ctx->socket);
        WSACleanup();
        return;
    }

    UI_AppendMessage(appState, "Connecting to server...");

    if (connect(ctx->socket,
                (struct sockaddr *)&serverAddr,
                sizeof(serverAddr)) == SOCKET_ERROR) {
        UI_AppendMessage(appState,
            "Connection failed. Is the server running?");
        closesocket(ctx->socket);
        WSACleanup();
        return;
    }

    /*
     * Mark connected before spawning the thread so the thread sees
     * isConnected == 1 the moment it starts running.
     */
    g_atomic_int_set(&ctx->isConnected, 1);
    UI_AppendMessage(appState, "Connected to server.");

    ctx->receiveThread = g_thread_new("recv_thread",
                                      Network_ReceiveLoop,
                                      appState);
}

/* ════════════════════════════════════════════════════════════════════════════
 * Network_SendMessage
 * ════════════════════════════════════════════════════════════════════════════ */

int Network_SendMessage(NetworkContext *ctx, const char *message) {
    g_assert(message != NULL);

    if (!g_atomic_int_get(&ctx->isConnected)) {
        return 0;
    }

    size_t length = strlen(message);

    if (length == 0) {
        return 0;
    }

    if (length >= CLIENT_BUFFER_SIZE) {
        fprintf(stderr,
            "[Client] Warning: message truncated to %d bytes.\n",
            CLIENT_BUFFER_SIZE - 1);
        length = CLIENT_BUFFER_SIZE - 1;
    }

    if (send(ctx->socket, message, (int)length, 0) == SOCKET_ERROR) {
        fprintf(stderr, "[Client] send() failed. Error: %d\n",
                WSAGetLastError());
        return 0;
    }

    return 1;
}

/* ════════════════════════════════════════════════════════════════════════════
 * Network_ReceiveLoop  (background thread)
 * ════════════════════════════════════════════════════════════════════════════ */

gpointer Network_ReceiveLoop(gpointer data) {
    ChatApplicationState *appState = (ChatApplicationState *)data;
    NetworkContext       *ctx      = &appState->network;

    char buffer[CLIENT_BUFFER_SIZE];

    while (g_atomic_int_get(&ctx->isConnected)) {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(ctx->socket,
                                 buffer,
                                 CLIENT_BUFFER_SIZE - 1,
                                 0);

        if (bytesReceived <= 0) {
            /* 0 = clean close by server; < 0 = network error */
            break;
        }

        buffer[bytesReceived] = '\0';

        /* Format and post to the UI thread safely */
        char *formatted = g_strdup_printf("Bot: %s", buffer);
        PostMessageToUI(appState, formatted);
        g_free(formatted);
    }

    /* Notify the UI that the connection has ended */
    g_atomic_int_set(&ctx->isConnected, 0);
    PostMessageToUI(appState, "Disconnected from server.");

    return NULL;
}

/* ════════════════════════════════════════════════════════════════════════════
 * Network_Disconnect
 * ════════════════════════════════════════════════════════════════════════════ */

void Network_Disconnect(NetworkContext *ctx) {
    /*
     * Clear the flag first so the receive thread exits its loop
     * naturally on the next iteration without reading a closed socket.
     */
    g_atomic_int_set(&ctx->isConnected, 0);

    if (ctx->socket != INVALID_SOCKET) {
        closesocket(ctx->socket);
        ctx->socket = INVALID_SOCKET;
    }

    WSACleanup();
}
