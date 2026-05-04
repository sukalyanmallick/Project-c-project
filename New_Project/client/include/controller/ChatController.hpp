/*
 * client/include/controller/ChatController.hpp — Client Controller (MVC)
 *
 * Responsibility: Ties the Model and View together. 
 * Listens for user input from the View, acts on the Model.
 * Listens to events from the Model, updates the View.
 */

#ifndef CHAT_CONTROLLER_HPP
#define CHAT_CONTROLLER_HPP

#include "../model/ChatModel.hpp"
#include "../view/ConsoleView.hpp"
#include <windows.h> // for Sleep

namespace ChatBot {
namespace Controller {

class ChatController {
public:
    ChatController(Model::ChatModel* model, View::ConsoleView* view);
    ~ChatController() = default;

    /* Start the main application loop */
    void run();

private:
    Model::ChatModel* model_;
    View::ConsoleView* view_;

    void setupBindings();
};

} // namespace Controller
} // namespace ChatBot

#endif /* CHAT_CONTROLLER_HPP */
