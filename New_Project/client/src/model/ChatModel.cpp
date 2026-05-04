/*
 * client/src/model/ChatModel.cpp — Client Model Implementation (MVC)
 */

#include "../../include/model/ChatModel.hpp"

namespace ChatBot {
namespace Model {

ChatModel::ChatModel(IConnectionManager* connector, IMessageSender* sender)
    : connector_(connector), sender_(sender) {}

void ChatModel::connectToServer() {
    if (onSystemEvent_) {
        onSystemEvent_("Connecting to server...");
    }
    connector_->connect();
}

void ChatModel::disconnectFromServer() {
    connector_->disconnect();
    if (onSystemEvent_) {
        onSystemEvent_("Disconnected from server.");
    }
}

bool ChatModel::sendMessage(const std::string& message) {
    if (message.empty()) return false;
    return sender_->sendMessage(message.c_str());
}

bool ChatModel::isConnected() const {
    return connector_->isConnected();
}

void ChatModel::setOnMessageReceived(std::function<void(std::string)> callback) {
    onMessageReceived_ = std::move(callback);
}

void ChatModel::setOnSystemEvent(std::function<void(std::string)> callback) {
    onSystemEvent_ = std::move(callback);
}

void ChatModel::onDataReceived(const char* data) {
    if (onMessageReceived_) {
        onMessageReceived_(data);
    }
}

} // namespace Model
} // namespace ChatBot
