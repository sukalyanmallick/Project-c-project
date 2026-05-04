/*
 * shared/constants.hpp — Shared Constants for GTK4 Chat Client & AI Chatbot Server
 *
 * Single source of truth for all configurable compile-time values.
 * Both the client and server include this file. To change the port
 * or buffer sizes, edit only this file.
 *
 * SOLID — Single Responsibility Principle:
 *   This file's sole responsibility is to centralise every magic
 *   number used across the project. Nothing else lives here.
 *
 * Sections:
 *   1. Network Configuration
 *   2. Buffer Sizes
 *   3. AI Engine
 *   4. Server Tuning
 */

#ifndef SHARED_CONSTANTS_HPP
#define SHARED_CONSTANTS_HPP

namespace ChatBot {

/* ─── 1. Network Configuration ───────────────────────────────────────────── */

constexpr const char* DEFAULT_SERVER_IP   = "127.0.0.1";
constexpr int         DEFAULT_SERVER_PORT = 5000;

/* ─── 2. Buffer Sizes ────────────────────────────────────────────────────── */

/*
 * CLIENT_BUFFER_SIZE: Maximum bytes the client reads in one recv() call.
 * SERVER_BUFFER_SIZE: Maximum bytes the server reads in one recv() call.
 *                     Kept slightly smaller to guarantee room for '\0'.
 */
constexpr int CLIENT_BUFFER_SIZE = 2048;
constexpr int SERVER_BUFFER_SIZE = 2000;

/* ─── 3. AI Engine ───────────────────────────────────────────────────────── */

/*
 * MAX_GENERIC_REPLIES: Size of the fallback reply pool in AiRandomStrategy.
 * Must stay in sync with the genericReplies[] array in ai_random.cpp.
 */
constexpr int MAX_GENERIC_REPLIES = 5;

/* ─── 4. Server Tuning ───────────────────────────────────────────────────── */

/*
 * SERVER_LISTEN_BACKLOG: Pending-connection queue depth passed to listen().
 * Raise this when adding multi-client support.
 */
constexpr int SERVER_LISTEN_BACKLOG = 3;

} // namespace ChatBot

#endif /* SHARED_CONSTANTS_HPP */
