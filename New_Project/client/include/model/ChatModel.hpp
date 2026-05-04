/*
 * client/include/model/ChatModel.hpp — Client Model (MVC)
 *
 * Responsibility: Manages the application state and business logic.
 * Wraps the network interfaces and provides a clean event-driven API
 * (callbacks) for the Controller to listen to.
 *
 * The Model knows nothing about the View or the Controller.
 */

#ifndef CHAT_MODEL_HPP
#define CHAT_MODEL_HPP

#include "../../include/INetworkClient.hpp"
#include <string>
#include <functional>
#include <memory>

namespace ChatBot {
namespace Model {

class ChatModel : public IDataReceiver {
public:
    /* Inject the network dependencies via interfaces */
    ChatModel(IConnectionManager* connector, IMessageSender* sender);
    ~ChatModel() = default;

    /* Actions */
    void connectToServer();
    void disconnectFromServer();
    bool sendMessage(const std::string& message);
    bool isConnected() const;

    /* Events (Controller registers to these) */
    void setOnMessageReceived(std::function<void(std::string)> callback);
    void setOnSystemEvent(std::function<void(std::string)> callback);

    /* Called by the network receive loop when data arrives */
    void onDataReceived(const char* data) override;

private:
    IConnectionManager* connector_;
    IMessageSender*     sender_;

    std::function<void(std::string)> onMessageReceived_;
    std::function<void(std::string)> onSystemEvent_;
};

} // namespace Model
} // namespace ChatBot

#endif /* CHAT_MODEL_HPP */
