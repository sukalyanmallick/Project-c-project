/*
 * client/include/view/ConsoleView.hpp — Client View (MVC)
 *
 * Responsibility: Handles all user interface operations (std::cin, std::cout).
 * The View knows absolutely nothing about networks, sockets, or logic.
 * It simply displays what it is told to display and captures what the user types.
 */

#ifndef CONSOLE_VIEW_HPP
#define CONSOLE_VIEW_HPP

#include <string>

namespace ChatBot {
namespace View {

class ConsoleView {
public:
    ConsoleView() = default;
    ~ConsoleView() = default;

    /* Display methods */
    void showWelcomeMessage() const;
    void showMessage(const std::string& sender, const std::string& message) const;
    void showSystemMessage(const std::string& message) const;
    void showError(const std::string& error) const;

    /* Input methods */
    std::string getUserInput() const;
};

} // namespace View
} // namespace ChatBot

#endif /* CONSOLE_VIEW_HPP */
