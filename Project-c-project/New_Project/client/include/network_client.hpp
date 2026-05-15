/*
 * client/include/network_client.hpp — Standard C++ Network Client
 *
 * This version uses std::thread and std::atomic, making it compatible
 * with any standard C++ compiler on Windows without GLib dependencies.
 */

#ifndef NETWORK_CLIENT_HPP
#define NETWORK_CLIENT_HPP

#include "INetworkClient.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <atomic>

namespace ChatBot {

class TcpNetworkClient final : public IConnectionManager,
                               public IMessageSender {
public:
    TcpNetworkClient();
    ~TcpNetworkClient() override;

    TcpNetworkClient(const TcpNetworkClient&)            = delete;
    TcpNetworkClient& operator=(const TcpNetworkClient&) = delete;

    /* ── IConnectionManager ── */
    void connect()             override;
    void disconnect()          override;
    bool isConnected()   const override;

    /* ── IMessageSender ── */
    bool sendMessage(const char* message) override;

    /* ── IDataReceiver Registration ── */
    void setReceiver(IDataReceiver* receiver);

private:
    IDataReceiver*        receiver_;
    SOCKET                socket_;
    std::atomic<bool>     isConnected_;
    HANDLE                receiveThread_;

    static struct sockaddr_in buildServerAddress();
    static DWORD WINAPI receiveLoopEntry(LPVOID lpParam);
    void receiveLoop();
};

} // namespace ChatBot

#endif /* NETWORK_CLIENT_HPP */
