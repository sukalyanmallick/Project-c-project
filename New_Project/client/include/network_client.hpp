/*
 * client/include/network_client.hpp — Standard C++ Network Client
 *
 * This version uses std::thread and std::atomic, making it compatible
 * with any standard C++ compiler on Windows without GLib dependencies.
 */

#ifndef NETWORK_CLIENT_HPP
#define NETWORK_CLIENT_HPP

#include "INetworkClient.hpp"
#include "types.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <atomic>

namespace ChatBot {

class TcpNetworkClient final : public IConnectionManager,
                               public IMessageSender {
public:
    explicit TcpNetworkClient(ChatApplicationState* appState);
    ~TcpNetworkClient() override;

    TcpNetworkClient(const TcpNetworkClient&)            = delete;
    TcpNetworkClient& operator=(const TcpNetworkClient&) = delete;

    /* ── IConnectionManager ── */
    void connect()             override;
    void disconnect()          override;
    bool isConnected()   const override;

    /* ── IMessageSender ── */
    bool sendMessage(const char* message) override;

private:
    ChatApplicationState* appState_;
    SOCKET                socket_;
    std::atomic<bool>     isConnected_;
    HANDLE                receiveThread_;

    static struct sockaddr_in buildServerAddress();
    void postMessageToUI(const char* text);
    static DWORD WINAPI receiveLoopEntry(LPVOID lpParam);
    void receiveLoop();
};

} // namespace ChatBot

#endif /* NETWORK_CLIENT_HPP */
