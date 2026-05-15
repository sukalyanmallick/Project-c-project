/*
 * client/src/console_main.cpp — Standard C++ Console Chat Client (MVC)
 *
 * This file is the Composition Root. It instantiates the Model, View, and Controller,
 * wires them together, and hands over execution to the Controller.
 */

#include "../include/network_client.hpp"
#include "../include/model/ChatModel.hpp"
#include "../include/view/ConsoleView.hpp"
#include "../include/controller/ChatController.hpp"

int main() {
    using namespace ChatBot;

    // 1. Create Network Implementation
    auto* network = new TcpNetworkClient();
    
    // 2. Create Model, passing network abstractions
    auto* model = new Model::ChatModel(network, network);
    
    // Wire the network to send data back to the model
    network->setReceiver(model);
    
    // 3. Create View
    auto* view = new View::ConsoleView();
    
    // 4. Create Controller
    auto* controller = new Controller::ChatController(model, view);

    // 5. Run the application
    controller->run();

    // 6. Cleanup
    delete controller;
    delete view;
    delete model;
    delete network;

    return 0;
}
