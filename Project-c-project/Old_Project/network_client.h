/*
 * network_client.h — Client Network Layer Interface
 *
 * Declares functions that manage the Winsock2 TCP connection from the
 * GTK4 chat client to the AI chatbot server.
 *
 * The NetworkContext struct holds all socket and thread state.
 * It is embedded inside ChatApplicationState so that network state
 * is never stored in global variables.
 *
 * Thread safety note:
 *   isConnected is a gint accessed atomically via g_atomic_int_get/set
 *   to prevent race conditions between the GTK main thread and the
 *   background receive thread.
 */

#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <glib.h>

/* ─── NetworkContext ─────────────────────────────────────────────────────── */

typedef struct {
    SOCKET  socket;         /* Connected socket to the chatbot server            */
    gint    isConnected;    /* Atomic flag: 1 = connected, 0 = disconnected       */
    GThread *receiveThread; /* Background thread running Network_ReceiveLoop()    */
} NetworkContext;

/* ─── Forward declaration (defined in ui.h) ─────────────────────────────── */
typedef struct ChatApplicationState ChatApplicationState;

/* ─── Network Lifecycle ──────────────────────────────────────────────────── */

/*
 * Network_Connect — Initialize Winsock, create a TCP socket, and connect
 * to DEFAULT_SERVER_IP:DEFAULT_SERVER_PORT.
 *
 * On success, sets ctx->isConnected = 1 and spawns the receive thread.
 * On failure, appends an error message to the chat view and cleans up.
 *
 * Parameters:
 *   appState — Full application state; used to pass context to the receive
 *              thread and to call UI_AppendMessage() on errors.
 */
void Network_Connect(ChatApplicationState *appState);

/*
 * Network_SendMessage — Send a null-terminated string to the server.
 *
 * Validates that the connection is active and the message is non-empty
 * before calling send(). Input length is capped at CLIENT_BUFFER_SIZE - 1.
 *
 * Parameters:
 *   ctx     — Active NetworkContext (isConnected must be 1).
 *   message — Null-terminated message string. Must not be NULL.
 *
 * Returns:
 *   1 on success, 0 on send failure or disconnected state.
 */
int Network_SendMessage(NetworkContext *ctx, const char *message);

/*
 * Network_ReceiveLoop — Background thread entry point.
 *
 * Blocks on recv() in a loop and posts each incoming message to the GTK
 * main thread via g_idle_add() for safe UI updates.
 * Exits when isConnected is set to 0 or recv() returns an error.
 *
 * Parameters:
 *   data — Pointer to the ChatApplicationState (cast from gpointer).
 *
 * Returns:
 *   NULL (GThread convention).
 */
gpointer Network_ReceiveLoop(gpointer data);

/*
 * Network_Disconnect — Close the socket and clean up Winsock.
 *
 * Sets isConnected = 0 atomically before closing so the receive thread
 * can exit cleanly. Safe to call if already disconnected.
 *
 * Parameters:
 *   ctx — Pointer to the NetworkContext to disconnect.
 */
void Network_Disconnect(NetworkContext *ctx);

#endif /* NETWORK_CLIENT_H */
