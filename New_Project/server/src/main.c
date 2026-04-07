/*
 * server/src/main.c — AI Chatbot Server Entry Point
 *
 * Drives the server through its four lifecycle phases:
 *
 *   Phase 1  Server_Initialize       — Winsock, socket, bind, listen
 *   Phase 2  Server_WaitForClient    — blocking accept()
 *   Phase 3  Server_HandleClientSession — recv / AI / send loop
 *   Phase 4  Server_Shutdown         — close sockets, WSACleanup
 *
 * To support multiple simultaneous clients, wrap phases 2–3 in a loop
 * and dispatch each accepted socket to a new thread.
 */

#include "../include/network_server.h"
#include "../include/ai_engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    printf("[Server] AI Chatbot Server starting...\n");

    /*
     * Seed once at startup so AI_RandomStrategy() produces varied replies
     * across different server runs.
     */
    srand((unsigned int)time(NULL));

    ServerContext ctx;

    if (!Server_Initialize(&ctx)) {
        fprintf(stderr, "[Server] Initialization failed. Exiting.\n");
        return EXIT_FAILURE;
    }

    if (!Server_WaitForClient(&ctx)) {
        fprintf(stderr, "[Server] Failed to accept client. Shutting down.\n");
        Server_Shutdown(&ctx);
        return EXIT_FAILURE;
    }

    Server_HandleClientSession(&ctx);
    Server_Shutdown(&ctx);

    return EXIT_SUCCESS;
}
