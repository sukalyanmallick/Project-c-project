/*
 * client/include/INetworkClient.hpp — Client Network Interface Segregation (C++ / SOLID)
 *
 * SOLID Principles Applied:
 *
 *   Interface Segregation Principle (ISP):
 *     The original NetworkContext mixed connection management, sending,
 *     and receiving into one place. Here we split into two narrow
 *     interfaces so that callers only depend on what they actually use:
 *
 *       IConnectionManager — connect() / disconnect()
 *       IMessageSender     — sendMessage()
 *
 *     The receive loop is a GThread detail internal to TcpNetworkClient;
 *     it is not exposed as an interface because no other component calls it.
 *
 *   Dependency Inversion Principle (DIP):
 *     UI callbacks depend on IConnectionManager* and IMessageSender*,
 *     never on the concrete TcpNetworkClient class.
 *
 *   Open/Closed Principle (OCP):
 *     A mock or TLS network client can implement these interfaces for
 *     testing or extension without touching any UI code.
 */

#ifndef INETWORK_CLIENT_HPP
#define INETWORK_CLIENT_HPP

namespace ChatBot {

/* ─── IConnectionManager ─────────────────────────────────────────────────── */

/*
 * Controls the lifecycle of the network connection.
 * Used by: UI_OnConnectClicked, UI_OnWindowClose.
 */
class IConnectionManager {
public:
    /*
     * connect — Initialise Winsock, create socket, connect to server,
     *           spawn the receive thread, and update the UI.
     */
    virtual void connect()    = 0;

    /*
     * disconnect — Atomically clear the connected flag, close the socket,
     *              and clean up Winsock. Safe to call if already disconnected.
     */
    virtual void disconnect() = 0;

    /* isConnected — Thread-safe query of the connection state. */
    virtual bool isConnected() const = 0;

    virtual ~IConnectionManager() = default;
};

/* ─── IMessageSender ─────────────────────────────────────────────────────── */

/*
 * Sends a single message to the server.
 * Used by: UI_OnSendClicked.
 */
class IMessageSender {
public:
    /*
     * sendMessage — Validate and send a null-terminated string to the server.
     *
     * Parameters:
     *   message — Null-terminated string. Must not be nullptr.
     *
     * Returns:
     *   true on success, false if not connected or send failed.
     */
    virtual bool sendMessage(const char* message) = 0;

    virtual ~IMessageSender() = default;
};

} // namespace ChatBot

#endif /* INETWORK_CLIENT_HPP */
