/*
 * ai_engine.h — AI Reply Engine Interface
 *
 * Declares the Strategy-pattern-based reply functions used by the server
 * to generate responses. Each strategy handles a different category of input.
 *
 * The primary entry point is AI_GenerateReply(), which selects the appropriate
 * strategy automatically based on the input message content.
 */

#ifndef AI_ENGINE_H
#define AI_ENGINE_H

/* ─── Reply Strategy Function Type ──────────────────────────────────────── */

/*
 * A ReplyStrategy is a function that takes an input string and returns
 * a statically allocated or caller-managed reply string.
 * Using function pointers allows new strategies to be plugged in without
 * modifying AI_GenerateReply().
 */
typedef const char* (*ReplyStrategy)(const char *inputMessage);

/* ─── Individual Strategy Functions ─────────────────────────────────────── */

/*
 * AI_KeywordStrategy — Matches greetings, questions, and known keywords.
 * Returns NULL if the input does not match any known keyword.
 */
const char* AI_KeywordStrategy(const char *inputMessage);

/*
 * AI_TimeStrategy — Returns the current local time if "time" is in the input.
 * Uses a static internal buffer; result is valid until next call.
 * Returns NULL if "time" is not found in the input.
 */
const char* AI_TimeStrategy(const char *inputMessage);

/*
 * AI_RandomStrategy — Returns a random generic fallback response.
 * Always succeeds. Call srand() once before using this.
 */
const char* AI_RandomStrategy(const char *inputMessage);

/* ─── Primary Entry Point ────────────────────────────────────────────────── */

/*
 * AI_GenerateReply — Selects and runs the appropriate strategy for the input.
 *
 * Strategy priority:
 *   1. AI_KeywordStrategy  (exact keyword matching)
 *   2. AI_TimeStrategy     (time query)
 *   3. AI_RandomStrategy   (generic fallback)
 *
 * Parameters:
 *   inputMessage — null-terminated string received from the client.
 *                  Must not be NULL.
 *
 * Returns:
 *   A null-terminated reply string. The caller must NOT free this pointer;
 *   it points to static storage managed by the strategy functions.
 */
const char* AI_GenerateReply(const char *inputMessage);

#endif /* AI_ENGINE_H */
