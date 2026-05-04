/*
 * server/include/IServerLifecycle.hpp — Server Lifecycle Interface
 *
 * SOLID Principles Applied:
 *
 *   Interface Segregation Principle (ISP):
 *     The four server phases are grouped here because they always move
 *     together (init → wait → handle → shutdown). Any future split
 *     (e.g. separating bind from listen) would produce narrower interfaces.
 *
 *   Open/Closed Principle (OCP):
 *     A multi-client or TLS server can implement this interface without
 *     touching main.cpp or any other existing file.
 *
 *   Dependency Inversion Principle (DIP):
 *     main.cpp depends on IServerLifecycle*, not on the concrete TcpServer.
 *
 * Typical usage in main():
 *
 *   std::unique_ptr<IServerLifecycle> server = std::make_unique<TcpServer>(engine);
 *   if (!server->initialize())       { return 1; }
 *   if (!server->waitForClient())    { server->shutdown(); return 1; }
 *   server->handleClientSession();
 *   server->shutdown();
 */

#ifndef ISERVER_LIFECYCLE_HPP
#define ISERVER_LIFECYCLE_HPP

namespace ChatBot {

class IServerLifecycle {
public:
    /* Phase 1: Start Winsock, create socket, bind, listen. */
    virtual bool initialize()          = 0;

    /* Phase 2: Block until one client connects. */
    virtual bool waitForClient()       = 0;

    /* Phase 3: Run the recv / AI / send loop for the active client. */
    virtual void handleClientSession() = 0;

    /* Phase 4: Close all sockets and clean up Winsock. */
    virtual void shutdown()            = 0;

    virtual ~IServerLifecycle() = default;
};

} // namespace ChatBot

#endif /* ISERVER_LIFECYCLE_HPP */
