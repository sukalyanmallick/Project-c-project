/*
 * server/include/ai_engine.hpp — AI Reply Engine (C++ / SOLID)
 *
 * SOLID Principles Applied:
 *
 *   Single Responsibility (SRP):
 *     AiEngine has one job: walk a priority-ordered list of strategies
 *     and return the first match. It knows nothing about keywords, time,
 *     or random replies.
 *
 *   Open/Closed (OCP):
 *     New strategies are registered via addStrategy() — AiEngine's own
 *     source code never changes when behaviour is extended.
 *
 *   Dependency Inversion (DIP):
 *     AiEngine depends on IReplyStrategy*, an abstraction, not on any
 *     concrete class.
 *
 * Usage:
 *   AiEngine engine;
 *   engine.addStrategy(std::make_unique<AiKeywordStrategy>());
 *   engine.addStrategy(std::make_unique<AiTimeStrategy>());
 *   engine.addStrategy(std::make_unique<AiRandomStrategy>());
 *   const char* reply = engine.generateReply("hello");
 */

#ifndef AI_ENGINE_HPP
#define AI_ENGINE_HPP

#include "IReplyStrategy.hpp"

#include <vector>
#include <memory>

namespace ChatBot {

class AiEngine {
public:
    AiEngine()  = default;
    ~AiEngine() = default;

    /* Non-copyable — owns unique_ptr resources. */
    AiEngine(const AiEngine&)            = delete;
    AiEngine& operator=(const AiEngine&) = delete;

    /*
     * addStrategy — Register a strategy in priority order.
     *
     * Strategies are tried in insertion order; the first non-nullptr
     * result wins. The last registered strategy should be an
     * unconditional fallback (e.g. AiRandomStrategy).
     *
     * Parameters:
     *   strategy — Owning pointer to a concrete IReplyStrategy.
     */
    void addStrategy(std::unique_ptr<IReplyStrategy> strategy);

    /*
     * generateReply — Select and run the correct strategy.
     *
     * Parameters:
     *   inputMessage — null-terminated string from the client. Must not be nullptr.
     *
     * Returns:
     *   A null-terminated reply string. Do not free the returned pointer.
     *   Falls back to a safe default if no strategy matches (should not happen
     *   when AiRandomStrategy is registered last).
     */
    const char* generateReply(const char* inputMessage) const;

private:
    std::vector<std::unique_ptr<IReplyStrategy>> strategies_;
};

} // namespace ChatBot

#endif /* AI_ENGINE_HPP */
