/*
 * server/include/view/ServerLogger.hpp — Server View (MVC)
 *
 * Responsibility: Handles all console output for the server.
 * This encapsulates how the server displays its internal state and events
 * (the "View" in a headless server environment).
 */

#ifndef SERVER_LOGGER_HPP
#define SERVER_LOGGER_HPP

#include <string>

namespace ChatBot {
namespace View {

class ServerLogger {
public:
    ServerLogger() = default;
    ~ServerLogger() = default;

    void logInfo(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logError(const std::string& message) const;
    
    // Domain-specific logging
    void logClientConnected(const std::string& ip) const;
    void logMessageReceived(const std::string& msg) const;
    void logReplySent(const std::string& reply) const;
};

} // namespace View
} // namespace ChatBot

#endif /* SERVER_LOGGER_HPP */
