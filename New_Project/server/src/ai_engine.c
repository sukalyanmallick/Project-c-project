/*
 * server/src/ai_engine.c — AI Reply Dispatcher
 *
 * Implements AI_GenerateReply(), the single public entry point that
 * selects and runs the appropriate reply strategy.
 *
 * The strategy table is the only place that knows about individual
 * strategies. To add or remove a strategy, edit strategyTable[] here
 * — the individual strategy files are untouched.
 *
 * Invariant: the last entry in strategyTable[] must always return
 * a non-NULL value (i.e. it must be the unconditional fallback).
 */

#include "../include/ai_engine.h"

#include <assert.h>
#include <stddef.h>

/* ─── Strategy Dispatch Table ────────────────────────────────────────────── */

/*
 * Strategies are tried in declaration order. The first non-NULL result
 * is returned to the caller. AI_RandomStrategy is the guaranteed
 * catch-all and must remain last.
 */
static const ReplyStrategy strategyTable[] = {
    AI_KeywordStrategy,     /* 1st: exact keyword / greeting matching */
    AI_TimeStrategy,        /* 2nd: time-query detection              */
    AI_RandomStrategy       /* 3rd: unconditional fallback            */
};

static const int STRATEGY_COUNT =
    (int)(sizeof(strategyTable) / sizeof(strategyTable[0]));

/* ─── AI_GenerateReply ───────────────────────────────────────────────────── */

const char *AI_GenerateReply(const char *inputMessage) {
    assert(inputMessage != NULL);

    for (int i = 0; i < STRATEGY_COUNT; i++) {
        const char *reply = strategyTable[i](inputMessage);
        if (reply != NULL) {
            return reply;
        }
    }

    /*
     * Unreachable: AI_RandomStrategy is always the last entry and
     * always returns a valid string. This line silences compiler
     * warnings and acts as a defensive last resort.
     */
    return "I didn't understand that. Could you try again?";
}
