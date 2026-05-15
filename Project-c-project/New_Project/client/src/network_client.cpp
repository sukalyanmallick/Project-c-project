/*
 * client/src/network_client.cpp — Standard C++ Network Client Implementation
 */

#include "../include/network_client.hpp"
#include "../../shared/constants.hpp"

#include <cstdio>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

namespace ChatBot {

TcpNetworkClient::TcpNetworkClient()
    : receiver_(nullptr),
      socket_(INVALID_SOCKET),
      isConnected_(false),
      receiveThread_(nullptr)
{}

TcpNetworkClient::~TcpNetworkClient() {
    disconnect();
    if (receiveThread_) {
        WaitForSingleObject(receiveThread_, INFINITE);
        CloseHandle(receiveThread_);
        receiveThread_ = nullptr;
    }
}

struct sockaddr_in TcpNetworkClient::buildServerAddress() {
    struct sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(DEFAULT_SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(DEFAULT_SERVER_IP);
    return addr;
}

void TcpNetworkClient::setReceiver(IDataReceiver* receiver) {
    receiver_ = receiver;
}

void TcpNetworkClient::connect() {
    if (isConnected_) return;

    WSADATA wsaData{};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        if (receiver_) receiver_->onDataReceived("WSAStartup failed.");
        return;
    }

    socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET) {
        if (receiver_) receiver_->onDataReceived("Socket creation failed.");
        WSACleanup();
        return;
    }

    struct sockaddr_in serverAddr = buildServerAddress();
    if (serverAddr.sin_addr.s_addr == INADDR_NONE) {
        if (receiver_) receiver_->onDataReceived("Invalid IP.");
        closesocket(socket_);
        WSACleanup();
        return;
    }

    if (::connect(socket_, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        if (receiver_) receiver_->onDataReceived("Connection failed.");
        closesocket(socket_);
        WSACleanup();
        return;
    }

    isConnected_ = true;
    if (receiver_) receiver_->onDataReceived("Connected to server.");

    receiveThread_ = CreateThread(nullptr, 0, TcpNetworkClient::receiveLoopEntry, this, 0, nullptr);
}

void TcpNetworkClient::disconnect() {
    isConnected_ = false;
    if (socket_ != INVALID_SOCKET) {
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
    }
    WSACleanup();
}

bool TcpNetworkClient::isConnected() const {
    return isConnected_;
}

bool TcpNetworkClient::sendMessage(const char* message) {
    if (!isConnected_) return false;
    
    int len = static_cast<int>(std::strlen(message));
    if (send(socket_, message, len, 0) == SOCKET_ERROR) {
        return false;
    }
    return true;
}

DWORD WINAPI TcpNetworkClient::receiveLoopEntry(LPVOID lpParam) {
    auto* client = static_cast<TcpNetworkClient*>(lpParam);
    client->receiveLoop();
    return 0;
}

void TcpNetworkClient::receiveLoop() {
    char buffer[CLIENT_BUFFER_SIZE];
    while (isConnected_) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes = recv(socket_, buffer, CLIENT_BUFFER_SIZE - 1, 0);
        if (bytes <= 0) break;
        
        buffer[bytes] = '\0';
        if (receiver_) {
            receiver_->onDataReceived(buffer);
        }
    }
    isConnected_ = false;
    if (receiver_) receiver_->onDataReceived("[System] Disconnected from server.");
}

} // namespace ChatBot
