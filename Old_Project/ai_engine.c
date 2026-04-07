/*
 * ai_engine.c — AI Reply Engine Implementation
 *
 * Implements the Strategy pattern for generating chatbot replies.
 * Each strategy is a self-contained function that handles one category
 * of input. AI_GenerateReply() acts as the dispatcher.
 *
 * Adding a new reply category:
 *   1. Write a new strategy function matching the ReplyStrategy signature.
 *   2. Register it in the strategyTable[] array inside AI_GenerateReply().
 */

#include "ai_engine.h"
#include "../shared/constants.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* ─── AI_KeywordStrategy ─────────────────────────────────────────────────── */

const char* AI_KeywordStrategy(const char *inputMessage) {
    /*
     * Check for known keywords in the input using case-insensitive substring
     * matching. The order of checks matters — more specific checks come first.
     */
    if (strstr(inputMessage, "hi") || strstr(inputMessage, "hello")) {
        return "Hello! How can I help you today?";
    }

    if (strstr(inputMessage, "how are you")) {
        return "I'm doing great! Thanks for asking.";
    }

    if (strstr(inputMessage, "name")) {
        return "I'm your AI Chatbot Server!";
    }

    if (strstr(inputMessage, "bye")) {
        return "Goodbye! Have a great day!";
    }

    /* No keyword matched — signal the dispatcher to try the next strategy. */
    return NULL;
}

/* ─── AI_TimeStrategy ────────────────────────────────────────────────────── */

const char* AI_TimeStrategy(const char *inputMessage) {
    /*
     * Static buffer is safe here because the server is single-threaded.
     * If multi-threading is added, replace with a caller-provided buffer.
     */
    static char timeBuffer[100];

    if (strstr(inputMessage, "time")) {
        time_t currentTime = time(NULL);
        strftime(timeBuffer, sizeof(timeBuffer),
                 "Current time: %H:%M:%S", localtime(&currentTime));
        return timeBuffer;
    }

    return NULL;
}

/* ─── AI_RandomStrategy ──────────────────────────────────────────────────── */

const char* AI_RandomStrategy(const char *inputMessage) {
    /*
     * Fallback pool of generic responses used when no keyword or time
     * pattern is matched. srand() must be called once before this runs.
     * Array size must match MAX_GENERIC_REPLIES in constants.h.
     */
    (void)inputMessage; /* Unused — strategy always applies as a fallback. */

    static const char *genericReplies[MAX_GENERIC_REPLIES] = {
        "That's interesting! Tell me more.",
        "I'm not sure, but that sounds cool.",
        "Can you explain that a bit more?",
        "Hmm… I'll have to think about that.",
        "Good question! Let's learn together."
    };

    return genericReplies[rand() % MAX_GENERIC_REPLIES];
}

/* ─── AI_GenerateReply ───────────────────────────────────────────────────── */

const char* AI_GenerateReply(const char *inputMessage) {
    /*
     * Strategy dispatch table — strategies are tried in priority order.
     * The first non-NULL result wins. AI_RandomStrategy is the guaranteed
     * final fallback and always returns a valid string.
     */
    g_assert(inputMessage != NULL);

    ReplyStrategy strategyTable[] = {
        AI_KeywordStrategy,
        AI_TimeStrategy,
        AI_RandomStrategy
    };

    int strategyCount = sizeof(strategyTable) / sizeof(strategyTable[0]);

    for (int i = 0; i < strategyCount; i++) {
        const char *reply = strategyTable[i](inputMessage);
        if (reply != NULL) {
            return reply;
        }
    }

    /* Should never reach here — AI_RandomStrategy always returns a value. */
    return "I didn't understand that. Could you try again?";
}
