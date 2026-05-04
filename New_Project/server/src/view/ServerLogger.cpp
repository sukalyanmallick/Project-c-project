/*
 * server/src/view/ServerLogger.cpp — Server View Implementation
 */

#include "../../include/view/ServerLogger.hpp"
#include <iostream>

namespace ChatBot {
namespace View {

void ServerLogger::logInfo(const std::string& message) const {
    std::cout << "[Server] " << message << std::endl;
}

void ServerLogger::logWarning(const std::string& message) const {
    std::cout << "[Server WARNING] " << message << std::endl;
}

void ServerLogger::logError(const std::string& message) const {
    std::cerr << "[Server ERROR] " << message << std::endl;
}

void ServerLogger::logClientConnected(const std::string& ip) const {
    std::cout << "[Server] Client connected from " << ip << std::endl;
}

void ServerLogger::logMessageReceived(const std::string& msg) const {
    std::cout << "[Server] Received: " << msg << std::endl;
}

void ServerLogger::logReplySent(const std::string& reply) const {
    std::cout << "[Server] Replied: " << reply << std::endl;
}

} // namespace View
} // namespace ChatBot
