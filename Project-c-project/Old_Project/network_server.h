/*
 * network_server.h — Server Network Layer Interface
 *
 * Declares the server lifecycle functions that manage the Winsock2 TCP
 * socket from initialization through client handling to shutdown.
 *
 * Typical usage:
 *
 *   ServerContext ctx;
 *   if (!Server_Initialize(&ctx))   { return 1; }
 *   if (!Server_WaitForClient(&ctx)){ Server_Shutdown(&ctx); return 1; }
 *   Server_HandleClientSession(&ctx);
 *   Server_Shutdown(&ctx);
 */

#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

/* ─── Server Context ─────────────────────────────────────────────────────── */

/*
 * ServerContext holds all state needed for one server lifecycle.
 * Pass a pointer to this struct into every Server_* function rather
 * than relying on global variables.
 */
typedef struct {
    SOCKET  serverSocket;   /* Listening socket bound to DEFAULT_SERVER_PORT */
    SOCKET  clientSocket;   /* Socket for the currently connected client      */
} ServerContext;

/* ─── Lifecycle Functions ────────────────────────────────────────────────── */

/*
 * Server_Initialize — Start Winsock, create and bind the listening socket.
 *
 * Parameters:
 *   ctx — Pointer to a zero-initialised ServerContext.
 *
 * Returns:
 *   1 (TRUE) on success, 0 (FALSE) on any failure.
 *   Prints a descriptive error to stderr on failure.
 */
int Server_Initialize(ServerContext *ctx);

/*
 * Server_WaitForClient — Block until a client connects.
 *
 * Parameters:
 *   ctx — Pointer to an initialised ServerContext (after Server_Initialize).
 *
 * Returns:
 *   1 (TRUE) when a client is accepted, 0 (FALSE) on error.
 */
int Server_WaitForClient(ServerContext *ctx);

/*
 * Server_HandleClientSession — Run the recv/reply loop for the connected client.
 *
 * Reads messages in a loop, calls AI_GenerateReply() for each, and sends the
 * result back. Exits when the client disconnects or sends "bye".
 *
 * Parameters:
 *   ctx — Pointer to a ServerContext with a valid clientSocket.
 */
void Server_HandleClientSession(ServerContext *ctx);

/*
 * Server_Shutdown — Close all sockets and clean up Winsock.
 *
 * Safe to call even if initialization was partial.
 *
 * Parameters:
 *   ctx — Pointer to the ServerContext to shut down.
 */
void Server_Shutdown(ServerContext *ctx);

#endif /* NETWORK_SERVER_H */
