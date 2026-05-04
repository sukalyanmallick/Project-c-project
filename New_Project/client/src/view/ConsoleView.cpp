/*
 * client/src/view/ConsoleView.cpp — Client View Implementation (MVC)
 */

#include "../../include/view/ConsoleView.hpp"
#include "../../../shared/constants.hpp"
#include <iostream>

namespace ChatBot {
namespace View {

void ConsoleView::showWelcomeMessage() const {
    std::cout << "=== AI Chatbot Console Client (MVC Architecture) ===" << std::endl;
}

void ConsoleView::showMessage(const std::string& sender, const std::string& message) const {
    std::cout << sender << ": " << message << std::endl;
}

void ConsoleView::showSystemMessage(const std::string& message) const {
    std::cout << "[System] " << message << std::endl;
}

void ConsoleView::showError(const std::string& error) const {
    std::cerr << "[Error] " << error << std::endl;
}

std::string ConsoleView::getUserInput() const {
    std::cout << "> ";
    std::string input;
    std::getline(std::cin, input);
    return input;
}

} // namespace View
} // namespace ChatBot
