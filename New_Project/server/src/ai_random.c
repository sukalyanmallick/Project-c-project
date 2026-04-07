/*
 * server/src/ai_random.c — Random Fallback Reply Strategy
 *
 * Implements AI_RandomStrategy(), the guaranteed final fallback that
 * always returns a valid reply string when no other strategy matches.
 *
 * Requirements:
 *   srand() must be called once at program startup (done in main.c)
 *   before any call to AI_RandomStrategy().
 *
 * Extending the reply pool:
 *   1. Add a new string to genericReplies[].
 *   2. Update MAX_GENERIC_REPLIES in shared/constants.h to match.
 */

#include "../include/ai_engine.h"
#include "../../shared/constants.h"

#include <stdlib.h>

/* ─── Reply Pool ─────────────────────────────────────────────────────────── */

static const char *genericReplies[MAX_GENERIC_REPLIES] = {
    "That's interesting! Tell me more.",
    "I'm not sure, but that sounds cool.",
    "Can you explain that a bit more?",
    "Hmm\u2026 I'll have to think about that.",
    "Good question! Let's learn together."
};

/* ─── AI_RandomStrategy ──────────────────────────────────────────────────── */

const char *AI_RandomStrategy(const char *inputMessage) {
    /* inputMessage is intentionally unused — this strategy is unconditional. */
    (void)inputMessage;

    return genericReplies[rand() % MAX_GENERIC_REPLIES];
}
