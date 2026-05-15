/*
 * main.c — AI Chatbot Server Entry Point
 *
 * Orchestrates the server lifecycle using the four-phase pattern:
 *   1. Initialize  — Start Winsock, bind, and listen.
 *   2. WaitForClient — Block until one client connects.
 *   3. HandleClientSession — Run the recv/reply loop.
 *   4. Shutdown    — Close all sockets and clean up Winsock.
 *
 * To extend to multi-client support, wrap phases 2–3 in a loop and
 * spawn a new thread for each accepted client.
 */

#include "network_server.h"
#include "ai_engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    printf("Starting AI Chatbot Server...\n");

    /*
     * Seed the random number generator once at startup.
     * Used by AI_RandomStrategy() to vary generic fallback responses.
     */
    srand((unsigned int)time(NULL));

    ServerContext serverCtx;

    /* Phase 1: Initialize Winsock, create socket, bind, and listen */
    if (!Server_Initialize(&serverCtx)) {
        fprintf(stderr, "Server initialization failed. Exiting.\n");
        return 1;
    }

    /* Phase 2: Block until a client connects */
    if (!Server_WaitForClient(&serverCtx)) {
        fprintf(stderr, "Failed to accept a client. Shutting down.\n");
        Server_Shutdown(&serverCtx);
        return 1;
    }

    /* Phase 3: Handle the connected client's session */
    Server_HandleClientSession(&serverCtx);

    /* Phase 4: Clean up all resources */
    Server_Shutdown(&serverCtx);

    return 0;
}
