/*
 * server/src/main.cpp — AI Chatbot Server Entry Point (C++ / SOLID)
 *
 * SOLID Principles Applied:
 *
 *   Dependency Inversion (DIP):
 *     main() depends only on IServerLifecycle and AiEngine abstractions.
 *     Concrete types (TcpServer, AiKeywordStrategy, etc.) are created
 *     here — this is the Composition Root.
 *
 *   Open/Closed (OCP):
 *     To add a new AI strategy: implement IReplyStrategy, then call
 *     engine.addStrategy() here. No other file changes.
 *
 * Composition Root responsibilities:
 *   1. Seed the RNG for AiRandomStrategy.
 *   2. Build the AiEngine and register all strategies in priority order.
 *   3. Build the TcpServer with the engine injected.
 *   4. Run the four lifecycle phases.
 */

#include "../include/controller/IServerLifecycle.hpp"
#include "../include/controller/network_server.hpp"
#include "../include/model/ai_engine.hpp"
#include "../include/model/IReplyStrategy.hpp"
#include "../include/view/ServerLogger.hpp"

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <memory>

/* Factory declarations — defined in each strategy translation unit. */
namespace ChatBot {
    std::unique_ptr<IReplyStrategy> makeKeywordStrategy();
    std::unique_ptr<IReplyStrategy> makeTimeStrategy();
    std::unique_ptr<IReplyStrategy> makeRandomStrategy();
}

int main() {
    using namespace ChatBot;

    View::ServerLogger logger;
    logger.logInfo("AI Chatbot Server starting...");

    /*
     * Seed once at startup so AiRandomStrategy produces varied replies
     * across different server runs.
     */
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    /* ── Composition Root: build and wire all dependencies ── */

    AiEngine engine;
    engine.addStrategy(makeKeywordStrategy()); /* Priority 1: keywords   */
    engine.addStrategy(makeTimeStrategy());    /* Priority 2: time query */
    engine.addStrategy(makeRandomStrategy());  /* Priority 3: fallback   */

    /*
     * TcpServer receives AiEngine by reference (DIP).
     * main() depends on the IServerLifecycle interface.
     */
    std::unique_ptr<IServerLifecycle> server =
        std::make_unique<TcpServer>(engine, &logger);

    /* ── Four-phase lifecycle ── */

    if (!server->initialize()) {
        logger.logError("Initialization failed. Exiting.");
        return EXIT_FAILURE;
    }

    if (!server->waitForClient()) {
        logger.logError("Failed to accept client. Shutting down.");
        server->shutdown();
        return EXIT_FAILURE;
    }

    server->handleClientSession();
    server->shutdown();

    return EXIT_SUCCESS;
}
