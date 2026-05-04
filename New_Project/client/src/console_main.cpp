/*
 * client/src/console_main.cpp — Standard C++ Console Chat Client
 */

#include "../include/network_client.hpp"
#include "../../shared/constants.hpp"
#include <iostream>
#include <string>
#include <windows.h>

namespace ChatBot {
    // Satisfy the forward declaration in network_client.cpp
    void UI_AppendMessage(ChatApplicationState*, const char* message) {
        std::cout << "[System] " << message << std::endl;
    }
}

int main() {
    using namespace ChatBot;

    std::cout << "=== AI Chatbot Console Client (Standard C++) ===" << std::endl;
    
    // 1. Setup State using standard new
    auto* appState = new ChatApplicationState();
    
    // 2. Create Network Client
    auto* network = new TcpNetworkClient(appState);
    
    std::cout << "Connecting to " << DEFAULT_SERVER_IP << ":" << DEFAULT_SERVER_PORT << "..." << std::endl;
    network->connect();

    // Wait for connection
    int retry = 0;
    while(!network->isConnected() && retry < 10) {
        Sleep(100);
        retry++;
    }

    if (network->isConnected()) {
        std::cout << "Connected! Type your message (type 'bye' to exit):" << std::endl;
        
        std::string input;
        while (network->isConnected()) {
            std::cout << "> ";
            std::getline(std::cin, input);
            
            if (input.empty()) continue;
            
            if (!network->sendMessage(input.c_str())) {
                break;
            }
            
            if (input == "bye") {
                network->disconnect();
                break;
            }
            
            // Wait a bit for the background thread to print the response
            Sleep(200); 
        }
    } else {
        std::cout << "Could not connect to server. Is it running?" << std::endl;
    }

    delete network;
    delete appState;

    std::cout << "Exiting client." << std::endl;
    return 0;
}
