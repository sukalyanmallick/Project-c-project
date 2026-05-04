/*
 * server/src/ai_random.cpp — Random Fallback Reply Strategy (C++ / SOLID)
 *
 * SOLID Principles Applied:
 *
 *   Single Responsibility (SRP):
 *     This class has exactly one reason to change: the pool of generic
 *     fallback replies. Nothing else.
 *
 *   Liskov Substitution (LSP):
 *     AiRandomStrategy honours the IReplyStrategy contract and extends
 *     the guarantee: it ALWAYS returns a valid string (never nullptr).
 *     It is the unconditional fallback — safe to register last in AiEngine.
 *
 *   Open/Closed (OCP):
 *     To extend the reply pool, add a string to genericReplies_ and
 *     update MAX_GENERIC_REPLIES in constants.hpp. AiEngine is untouched.
 *
 * Requirements:
 *   std::srand() must be called once at program startup (done in main.cpp)
 *   before any call to AiRandomStrategy::generateReply().
 *
 * Extending the reply pool:
 *   1. Add a new string to genericReplies_.
 *   2. Update ChatBot::MAX_GENERIC_REPLIES in shared/constants.hpp.
 */

#include "../include/IReplyStrategy.hpp"
#include "../../shared/constants.hpp"

#include <cstdlib>
#include <memory>

namespace ChatBot {

/* ─── AiRandomStrategy ───────────────────────────────────────────────────── */

class AiRandomStrategy final : public IReplyStrategy {
public:
    /*
     * generateReply — Return a random reply from the generic pool.
     *
     * inputMessage is intentionally unused — this strategy is unconditional.
     * Returns: always a valid, non-null const char*.
     */
    const char* generateReply(const char* inputMessage) const override {
        (void)inputMessage; /* Unconditional strategy — input is irrelevant. */

        return genericReplies_[std::rand() % MAX_GENERIC_REPLIES];
    }

private:
    /*
     * genericReplies_ — Compile-time pool of fallback messages.
     * Count must match MAX_GENERIC_REPLIES in constants.hpp.
     */
    static constexpr const char* genericReplies_[MAX_GENERIC_REPLIES] = {
        "That's interesting! Tell me more.",
        "I'm not sure, but that sounds cool.",
        "Can you explain that a bit more?",
        "Hmm\u2026 I'll have to think about that.",
        "Good question! Let's learn together."
    };
};

/* Out-of-class definition required for static constexpr array (C++14). */
constexpr const char* AiRandomStrategy::genericReplies_[MAX_GENERIC_REPLIES];

/*
 * Factory function — lets main.cpp register this strategy via
 * the IReplyStrategy interface without depending on the concrete class.
 */
std::unique_ptr<IReplyStrategy> makeRandomStrategy() {
    return std::make_unique<AiRandomStrategy>();
}

} // namespace ChatBot
