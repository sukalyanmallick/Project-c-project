/*
 * server/src/ai_keyword.cpp — Keyword-Matching Reply Strategy (C++ / SOLID)
 *
 * SOLID Principles Applied:
 *
 *   Single Responsibility (SRP):
 *     This class has exactly one reason to change: the set of keywords
 *     the chatbot recognises and their canned replies. Nothing else.
 *
 *   Liskov Substitution (LSP):
 *     AiKeywordStrategy honours the IReplyStrategy contract:
 *       - Returns a valid const char* on a keyword match.
 *       - Returns nullptr when no keyword is found.
 *     It is fully substitutable for IReplyStrategy* in AiEngine.
 *
 *   Open/Closed (OCP):
 *     Adding a new keyword means adding one if-block here. AiEngine
 *     and all other strategy files remain untouched.
 *
 * Adding a new keyword:
 *   1. Add a new if-block with strstr() and the desired reply.
 *   2. More specific phrases must come before shorter ones
 *      (e.g. "how are you" before "how").
 */

#include "../../include/model/IReplyStrategy.hpp"

#include <cstring>
#include <memory>

namespace ChatBot {

/* ─── AiKeywordStrategy ──────────────────────────────────────────────────── */

class AiKeywordStrategy final : public IReplyStrategy {
public:
    /*
     * generateReply — Match known keywords in priority order.
     *
     * Returns: a static reply string on match, nullptr otherwise.
     */
    const char* generateReply(const char* inputMessage) const override {
        /*
         * Order matters: check the most specific patterns first so a short
         * keyword like "hi" cannot shadow a longer phrase that contains it.
         */

        if (std::strstr(inputMessage, "how are you")) {
            return "I'm doing great! Thanks for asking.";
        }

        if (std::strstr(inputMessage, "hello") ||
            std::strstr(inputMessage, "hi")) {
            return "Hello! How can I help you today?";
        }

        if (std::strstr(inputMessage, "name")) {
            return "I'm your AI Chatbot Server!";
        }

        if (std::strstr(inputMessage, "bye")) {
            return "Goodbye! Have a great day!";
        }

        /* No keyword matched — signal the dispatcher to try the next strategy. */
        return nullptr;
    }
};

/*
 * Factory function — allows main.cpp to obtain an AiKeywordStrategy
 * without knowing its concrete class name, keeping DIP intact.
 */
std::unique_ptr<IReplyStrategy> makeKeywordStrategy() {
    return std::make_unique<AiKeywordStrategy>();
}

} // namespace ChatBot
