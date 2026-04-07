/*
 * server/include/ai_engine.h — AI Reply Engine Public Interface
 *
 * Exposes the strategy function type, individual strategy functions,
 * and the primary dispatcher that selects and runs the right strategy.
 *
 * Design:
 *   Each ReplyStrategy is a self-contained function. The dispatcher
 *   (AI_GenerateReply) walks a priority-ordered table and returns the
 *   first non-NULL result. New categories are added by writing a new
 *   strategy and inserting it into the table — existing code is
 *   untouched.
 *
 * Ownership:
 *   All returned strings point to static storage managed inside the
 *   strategy implementation. Callers must NOT free them.
 */

#ifndef AI_ENGINE_H
#define AI_ENGINE_H

/* ─── Strategy Function Type ─────────────────────────────────────────────── */

/*
 * ReplyStrategy — pointer to a function that inspects an input message
 * and returns a reply string, or NULL if it cannot handle the input.
 */
typedef const char *(*ReplyStrategy)(const char *inputMessage);

/* ─── Strategy: Keyword Matching ─────────────────────────────────────────── */

/*
 * AI_KeywordStrategy — Handles greetings and other known keywords.
 *
 * Returns: a static reply string on match, NULL otherwise.
 */
const char *AI_KeywordStrategy(const char *inputMessage);

/* ─── Strategy: Time Query ───────────────────────────────────────────────── */

/*
 * AI_TimeStrategy — Returns the current local time when "time" appears
 * in the input.
 *
 * Uses an internal static buffer; valid until the next call.
 * Returns: a formatted time string on match, NULL otherwise.
 *
 * Thread-safety note: not reentrant due to the static buffer.
 * Safe for the current single-threaded server design.
 */
const char *AI_TimeStrategy(const char *inputMessage);

/* ─── Strategy: Random Fallback ──────────────────────────────────────────── */

/*
 * AI_RandomStrategy — Returns a random generic reply.
 *
 * This is the guaranteed final fallback; it always returns a valid string.
 * Requires srand() to have been called once before first use.
 */
const char *AI_RandomStrategy(const char *inputMessage);

/* ─── Primary Dispatcher ─────────────────────────────────────────────────── */

/*
 * AI_GenerateReply — Select and run the correct strategy for inputMessage.
 *
 * Strategy priority (first match wins):
 *   1. AI_KeywordStrategy   — exact keyword matching
 *   2. AI_TimeStrategy      — time query detection
 *   3. AI_RandomStrategy    — generic fallback (always succeeds)
 *
 * Parameters:
 *   inputMessage — null-terminated string from the client. Must not be NULL.
 *
 * Returns:
 *   A null-terminated reply string. Do not free the returned pointer.
 */
const char *AI_GenerateReply(const char *inputMessage);

#endif /* AI_ENGINE_H */
