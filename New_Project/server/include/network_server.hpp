/*
 * server/include/network_server.hpp — TCP Server Class Interface (C++ / SOLID)
 *
 * SOLID Principles Applied:
 *
 *   Single Responsibility (SRP):
 *     TcpServer manages exactly one concern — the Winsock2 TCP socket
 *     lifecycle. AI logic is not here; it is injected via AiEngine&.
 *
 *   Open/Closed (OCP):
 *     TcpServer implements IServerLifecycle. A TLS or UDP variant can
 *     implement the same interface without modifying main.cpp.
 *
 *   Dependency Inversion (DIP):
 *     TcpServer receives AiEngine by reference (constructor injection).
 *     It depends on AiEngine's public interface, not on any concrete
 *     strategy class.
 *
 * All Winsock state (sockets) is private — callers only see the four
 * lifecycle methods from IServerLifecycle.
 */

#ifndef NETWORK_SERVER_HPP
#define NETWORK_SERVER_HPP

#include "IServerLifecycle.hpp"
#include "ai_engine.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

namespace ChatBot {

class TcpServer : public IServerLifecycle {
public:
    /*
     * Constructor — inject the AI engine (Dependency Inversion).
     *
     * Parameters:
     *   engine — Reference to the fully-configured AiEngine.
     *            The engine must outlive this TcpServer instance.
     */
    explicit TcpServer(AiEngine& engine);

    /*
     * Destructor — calls shutdown() if the server is still running
     * to ensure sockets are always released.
     */
    ~TcpServer() override;

    /* Non-copyable — owns OS socket handles. */
    TcpServer(const TcpServer&)            = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    /* ── IServerLifecycle ── */
    bool initialize()          override;
    bool waitForClient()       override;
    void handleClientSession() override;
    void shutdown()            override;

private:
    AiEngine& engine_;          /* Injected AI engine (not owned)        */
    SOCKET    serverSocket_;    /* Listening socket bound to SERVER_PORT  */
    SOCKET    clientSocket_;    /* Dedicated socket for connected client  */

    /* ── Internal helpers ── */
    static struct sockaddr_in buildServerAddress();
    static bool               sessionShouldExit(const char* message);
};

} // namespace ChatBot

#endif /* NETWORK_SERVER_HPP */
