/*
 * server/include/network_server.h — Server Network Layer Interface
 *
 * Declares the ServerContext struct and the four lifecycle functions
 * that drive the server from startup to shutdown.
 *
 * Typical usage in main():
 *
 *   ServerContext ctx;
 *   if (!Server_Initialize(&ctx))    { return 1; }
 *   if (!Server_WaitForClient(&ctx)) { Server_Shutdown(&ctx); return 1; }
 *   Server_HandleClientSession(&ctx);
 *   Server_Shutdown(&ctx);
 *
 * To add multi-client support, wrap phases 2–3 in a loop and spawn a
 * thread per accepted client.
 */

#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

/* ─── ServerContext ──────────────────────────────────────────────────────── */

/*
 * Holds all socket state for one server lifecycle.
 * Pass a pointer to this struct into every Server_* function so that
 * no network state lives in global variables.
 */
typedef struct {
    SOCKET serverSocket;    /* Listening socket bound to DEFAULT_SERVER_PORT */
    SOCKET clientSocket;    /* Dedicated socket for the connected client      */
} ServerContext;

/* ─── Phase 1: Initialization ────────────────────────────────────────────── */

/*
 * Server_Initialize — Start Winsock, create the TCP socket, bind, and listen.
 *
 * Parameters:
 *   ctx — Pointer to an uninitialised ServerContext. Will be zero-filled
 *         on entry so that Server_Shutdown() is safe on partial failure.
 *
 * Returns:
 *   1 on success, 0 on any failure (error written to stderr).
 */
int Server_Initialize(ServerContext *ctx);

/* ─── Phase 2: Client Acceptance ─────────────────────────────────────────── */

/*
 * Server_WaitForClient — Block until one client connects.
 *
 * Parameters:
 *   ctx — Pointer to an initialised ServerContext (post-Initialize).
 *
 * Returns:
 *   1 when a client is accepted, 0 on error.
 */
int Server_WaitForClient(ServerContext *ctx);

/* ─── Phase 3: Session Handling ──────────────────────────────────────────── */

/*
 * Server_HandleClientSession — Run the recv/reply loop for the active client.
 *
 * Reads messages, passes each to AI_GenerateReply(), and sends the
 * response back. Exits on client disconnect or a "bye" message.
 *
 * Parameters:
 *   ctx — Pointer to a ServerContext with a valid clientSocket.
 */
void Server_HandleClientSession(ServerContext *ctx);

/* ─── Phase 4: Shutdown ──────────────────────────────────────────────────── */

/*
 * Server_Shutdown — Close all sockets and clean up Winsock.
 *
 * Safe to call after a partial Server_Initialize() failure.
 *
 * Parameters:
 *   ctx — Pointer to the ServerContext to shut down.
 */
void Server_Shutdown(ServerContext *ctx);

#endif /* NETWORK_SERVER_H */
