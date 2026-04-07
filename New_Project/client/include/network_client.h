/*
 * client/include/network_client.h — Client Network Layer Interface
 *
 * Declares NetworkContext and the four network functions used by the
 * GTK4 chat client. All Winsock details are encapsulated here; the
 * UI layer never touches socket APIs directly.
 *
 * Thread model:
 *   The GTK main thread calls Network_Connect / Network_SendMessage /
 *   Network_Disconnect. A background GThread runs Network_ReceiveLoop
 *   and posts UI updates through g_idle_add() — never touching GTK
 *   widgets directly.
 *
 * Thread safety:
 *   NetworkContext.isConnected is a gint read and written exclusively
 *   through g_atomic_int_get() / g_atomic_int_set() to prevent data
 *   races between the two threads.
 */

#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <glib.h>

/* ─── NetworkContext ─────────────────────────────────────────────────────── */

typedef struct {
    SOCKET   socket;          /* TCP socket connected to the server             */
    gint     isConnected;     /* Atomic flag: 1 = connected, 0 = disconnected   */
    GThread *receiveThread;   /* Background thread running Network_ReceiveLoop  */
} NetworkContext;

/* ─── Forward Declaration ────────────────────────────────────────────────── */

/* Defined in client/include/ui.h — declared here to avoid circular includes. */
typedef struct ChatApplicationState ChatApplicationState;

/* ─── Network_Connect ────────────────────────────────────────────────────── */

/*
 * Initialise Winsock, create a TCP socket, and connect to
 * DEFAULT_SERVER_IP:DEFAULT_SERVER_PORT.
 *
 * On success: sets isConnected = 1 and spawns the receive thread.
 * On failure: appends an error message to the chat view and cleans up.
 *
 * Parameters:
 *   appState — Full application state, forwarded to the receive thread
 *              so it can post messages to the UI via g_idle_add().
 */
void Network_Connect(ChatApplicationState *appState);

/* ─── Network_SendMessage ────────────────────────────────────────────────── */

/*
 * Send a null-terminated string to the server.
 *
 * Input is validated and length-capped at CLIENT_BUFFER_SIZE - 1.
 *
 * Parameters:
 *   ctx     — Active NetworkContext (isConnected must be 1).
 *   message — Null-terminated string. Must not be NULL.
 *
 * Returns:
 *   1 on success, 0 on failure or if not connected.
 */
int Network_SendMessage(NetworkContext *ctx, const char *message);

/* ─── Network_ReceiveLoop ────────────────────────────────────────────────── */

/*
 * Background thread entry point (GThreadFunc signature).
 *
 * Blocks on recv() in a loop; posts each message to the GTK main thread
 * via g_idle_add(). Exits when isConnected becomes 0 or recv() fails.
 *
 * Parameters:
 *   data — Pointer to ChatApplicationState, cast from gpointer.
 *
 * Returns:
 *   NULL (GThread convention).
 */
gpointer Network_ReceiveLoop(gpointer data);

/* ─── Network_Disconnect ─────────────────────────────────────────────────── */

/*
 * Close the socket and clean up Winsock.
 *
 * Sets isConnected = 0 atomically before closing so the receive thread
 * can exit its loop cleanly. Safe to call if already disconnected.
 *
 * Parameters:
 *   ctx — Pointer to the NetworkContext to disconnect.
 */
void Network_Disconnect(NetworkContext *ctx);

#endif /* NETWORK_CLIENT_H */
