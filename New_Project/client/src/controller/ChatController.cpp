/*
 * client/src/controller/ChatController.cpp — Client Controller Implementation (MVC)
 */

#include "../../include/controller/ChatController.hpp"

namespace ChatBot {
namespace Controller {

ChatController::ChatController(Model::ChatModel* model, View::ConsoleView* view)
    : model_(model), view_(view) {
    setupBindings();
}

void ChatController::setupBindings() {
    // Bind Model events to View actions
    model_->setOnMessageReceived([this](const std::string& msg) {
        view_->showMessage("Bot", msg);
    });

    model_->setOnSystemEvent([this](const std::string& sysMsg) {
        view_->showSystemMessage(sysMsg);
    });
}

void ChatController::run() {
    view_->showWelcomeMessage();
    model_->connectToServer();

    // Wait for connection to establish
    int retry = 0;
    while (!model_->isConnected() && retry < 10) {
        Sleep(100);
        retry++;
    }

    if (model_->isConnected()) {
        view_->showSystemMessage("Connected! Type your message (type 'bye' to exit):");

        while (model_->isConnected()) {
            std::string input = view_->getUserInput();

            if (input.empty()) continue;

            if (!model_->sendMessage(input)) {
                view_->showError("Failed to send message.");
                break;
            }

            if (input == "bye") {
                model_->disconnectFromServer();
                break;
            }

            // Small delay to allow the background thread to print the response
            // before showing the next input prompt.
            Sleep(200);
        }
    } else {
        view_->showError("Could not connect to server. Is it running?");
    }
}

} // namespace Controller
} // namespace ChatBot
