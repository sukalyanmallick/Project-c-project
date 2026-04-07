/*
 * server/src/ai_keyword.c — Keyword-Matching Reply Strategy
 *
 * Implements AI_KeywordStrategy(), which handles the most common
 * user inputs: greetings, identity questions, farewells, and simple
 * conversational phrases.
 *
 * Adding a new keyword:
 *   1. Add a new if-block with the keyword and the desired reply.
 *   2. More specific checks must come before broader ones
 *      (e.g. "how are you" before "how").
 */

#include "../include/ai_engine.h"

#include <string.h>

/* ─── AI_KeywordStrategy ─────────────────────────────────────────────────── */

const char *AI_KeywordStrategy(const char *inputMessage) {
    /*
     * Order matters: check the most specific patterns first so a short
     * keyword like "hi" cannot shadow a longer phrase that contains it.
     */

    if (strstr(inputMessage, "how are you")) {
        return "I'm doing great! Thanks for asking.";
    }

    if (strstr(inputMessage, "hello") || strstr(inputMessage, "hi")) {
        return "Hello! How can I help you today?";
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
