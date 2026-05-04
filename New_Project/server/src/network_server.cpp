/*
 * server/src/network_server.cpp — TCP Server Implementation (C++ / SOLID)
 *
 * SOLID Principles Applied:
 *
 *   Single Responsibility (SRP):
 *     TcpServer manages only the Winsock2 socket lifecycle (bind, listen,
 *     accept, recv, send, close). AI logic is fully delegated to AiEngine.
 *
 *   Dependency Inversion (DIP):
 *     AiEngine is injected through the constructor. TcpServer calls
 *     engine_.generateReply() — it never names a concrete strategy class.
 *
 *   Open/Closed (OCP):
 *     TcpServer implements IServerLifecycle. A TLS variant or a
 *     multi-client server can implement the same interface in a separate
 *     class without touching this file.
 *
 * Error handling convention:
 *   All socket errors print the Winsock error code to stderr so failures
 *   can be diagnosed without an attached debugger.
 */

#include "../include/network_server.hpp"
#include "../../shared/constants.hpp"

#include <cstdio>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

namespace ChatBot {

/* ─── Constructor / Destructor ───────────────────────────────────────────── */

TcpServer::TcpServer(AiEngine& engine)
    : engine_(engine),
      serverSocket_(INVALID_SOCKET),
      clientSocket_(INVALID_SOCKET)
{}

TcpServer::~TcpServer() {
    /* Guarantee cleanup even if the caller forgets to call shutdown(). */
    shutdown();
}

/* ─── Internal Helpers ───────────────────────────────────────────────────── */

struct sockaddr_in TcpServer::buildServerAddress() {
    struct sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(DEFAULT_SERVER_PORT);
    return addr;
}

bool TcpServer::sessionShouldExit(const char* message) {
    return std::strstr(message, "bye") != nullptr;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Phase 1 — initialize
 * ═══════════════════════════════════════════════════════════════════════════ */

bool TcpServer::initialize() {
    WSADATA wsaData{};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::fprintf(stderr, "[Server] WSAStartup failed. Error: %d\n",
                     WSAGetLastError());
        return false;
    }

    serverSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket_ == INVALID_SOCKET) {
        std::fprintf(stderr, "[Server] socket() failed. Error: %d\n",
                     WSAGetLastError());
        WSACleanup();
        return false;
    }

    struct sockaddr_in serverAddr = buildServerAddress();

    if (bind(serverSocket_,
             reinterpret_cast<struct sockaddr*>(&serverAddr),
             sizeof(serverAddr)) == SOCKET_ERROR) {
        std::fprintf(stderr, "[Server] bind() failed. Error: %d\n",
                     WSAGetLastError());
        closesocket(serverSocket_);
        WSACleanup();
        return false;
    }

    if (listen(serverSocket_, SERVER_LISTEN_BACKLOG) == SOCKET_ERROR) {
        std::fprintf(stderr, "[Server] listen() failed. Error: %d\n",
                     WSAGetLastError());
        closesocket(serverSocket_);
        WSACleanup();
        return false;
    }

    std::printf("[Server] Listening on port %d...\n", DEFAULT_SERVER_PORT);
    return true;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Phase 2 — waitForClient
 * ═══════════════════════════════════════════════════════════════════════════ */

bool TcpServer::waitForClient() {
    std::printf("[Server] Waiting for a client...\n");

    struct sockaddr_in clientAddr{};
    int addrLen = static_cast<int>(sizeof(clientAddr));

    clientSocket_ = accept(serverSocket_,
                           reinterpret_cast<struct sockaddr*>(&clientAddr),
                           &addrLen);

    if (clientSocket_ == INVALID_SOCKET) {
        std::fprintf(stderr, "[Server] accept() failed. Error: %d\n",
                     WSAGetLastError());
        return false;
    }

    std::printf("[Server] Client connected.\n");
    return true;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Phase 3 — handleClientSession
 * ═══════════════════════════════════════════════════════════════════════════ */

void TcpServer::handleClientSession() {
    char incomingMessage[SERVER_BUFFER_SIZE];
    char outgoingReply  [SERVER_BUFFER_SIZE];

    while (true) {
        std::memset(incomingMessage, 0, sizeof(incomingMessage));

        int bytesReceived = recv(clientSocket_,
                                 incomingMessage,
                                 SERVER_BUFFER_SIZE - 1,
                                 0);

        if (bytesReceived <= 0) {
            std::printf("[Server] Client disconnected.\n");
            break;
        }

        incomingMessage[bytesReceived] = '\0';
        std::printf("[Server] Client: %s\n", incomingMessage);

        /*
         * Delegate to AiEngine (DIP) — TcpServer knows nothing about
         * which strategies are registered.
         */
        const char* aiResponse = engine_.generateReply(incomingMessage);
        std::strncpy(outgoingReply, aiResponse, SERVER_BUFFER_SIZE - 1);
        outgoingReply[SERVER_BUFFER_SIZE - 1] = '\0';

        if (send(clientSocket_,
                 outgoingReply,
                 static_cast<int>(std::strlen(outgoingReply)),
                 0) == SOCKET_ERROR) {
            std::fprintf(stderr, "[Server] send() failed. Error: %d\n",
                         WSAGetLastError());
            break;
        }

        if (sessionShouldExit(incomingMessage)) {
            std::printf("[Server] Session ended by client.\n");
            break;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Phase 4 — shutdown
 * ═══════════════════════════════════════════════════════════════════════════ */

void TcpServer::shutdown() {
    if (clientSocket_ != INVALID_SOCKET) {
        closesocket(clientSocket_);
        clientSocket_ = INVALID_SOCKET;
    }

    if (serverSocket_ != INVALID_SOCKET) {
        closesocket(serverSocket_);
        serverSocket_ = INVALID_SOCKET;
    }

    WSACleanup();
    std::printf("[Server] Shutdown complete.\n");
}

} // namespace ChatBot
