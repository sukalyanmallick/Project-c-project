/*
 * constants.h — Shared constants for GTK4 Chat Client & AI Chatbot Server
 *
 * All configurable values are defined here so that both the client and server
 * reference a single source of truth. To change the port or buffer sizes,
 * edit only this file.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/* ─── Network Configuration ─────────────────────────────────────────────── */

#define DEFAULT_SERVER_IP       "127.0.0.1"
#define DEFAULT_SERVER_PORT     5000

/* ─── Buffer Sizes ───────────────────────────────────────────────────────── */

/*
 * CLIENT_BUFFER_SIZE: Maximum bytes the client reads in one recv() call.
 * SERVER_BUFFER_SIZE: Maximum bytes the server reads in one recv() call.
 * Keep SERVER_BUFFER_SIZE slightly smaller to leave room for null-termination.
 */
#define CLIENT_BUFFER_SIZE      2048
#define SERVER_BUFFER_SIZE      2000

/* ─── AI Engine ──────────────────────────────────────────────────────────── */

/*
 * Number of generic fallback responses in the AI reply pool.
 * Must match the array size in ai_engine.c.
 */
#define MAX_GENERIC_REPLIES     5

/* ─── Server Backlog ─────────────────────────────────────────────────────── */

/*
 * Maximum number of pending connections the server will queue
 * while handling the current client.
 */
#define SERVER_LISTEN_BACKLOG   3

#endif /* CONSTANTS_H */
