/*
 * server/src/ai_engine.cpp — AI Reply Dispatcher Implementation (C++ / SOLID)
 *
 * SOLID Principles Applied:
 *
 *   Single Responsibility (SRP):
 *     AiEngine has one job: iterate registered strategies and return
 *     the first non-nullptr reply. It does not know about keywords,
 *     time, or random responses.
 *
 *   Open/Closed (OCP):
 *     addStrategy() allows new behaviour to be plugged in without
 *     modifying this file. AiEngine is closed for modification, open
 *     for extension through IReplyStrategy implementations.
 *
 *   Dependency Inversion (DIP):
 *     AiEngine iterates IReplyStrategy* — it never names a concrete class.
 *
 * Invariant:
 *   The last registered strategy should always return a non-nullptr value
 *   (i.e. be an unconditional fallback such as AiRandomStrategy).
 */

#include "../../include/model/ai_engine.hpp"

#include <cassert>

namespace ChatBot {

/* ─── addStrategy ────────────────────────────────────────────────────────── */

void AiEngine::addStrategy(std::unique_ptr<IReplyStrategy> strategy) {
    assert(strategy != nullptr && "Registered strategy must not be null.");
    strategies_.push_back(std::move(strategy));
}

/* ─── generateReply ──────────────────────────────────────────────────────── */

const char* AiEngine::generateReply(const char* inputMessage) const {
    assert(inputMessage != nullptr && "inputMessage must not be null.");

    for (const auto& strategy : strategies_) {
        const char* reply = strategy->generateReply(inputMessage);
        if (reply != nullptr) {
            return reply;
        }
    }

    /*
     * Defensive fallback — unreachable when AiRandomStrategy is registered
     * last, as it always returns a non-null string.
     */
    return "I didn't understand that. Could you try again?";
}

} // namespace ChatBot
