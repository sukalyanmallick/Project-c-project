/*
 * server/src/ai_time.c — Time-Query Reply Strategy
 *
 * Implements AI_TimeStrategy(), which detects the word "time" in the
 * input and responds with the current local time formatted as HH:MM:SS.
 *
 * Thread-safety note:
 *   timeBuffer is a function-scoped static. Safe for the current
 *   single-threaded server design. If multi-threading is added,
 *   replace with a caller-provided buffer or use localtime_r().
 */

#include "../include/ai_engine.h"

#include <string.h>
#include <time.h>
#include <stdio.h>

/* ─── AI_TimeStrategy ────────────────────────────────────────────────────── */

const char *AI_TimeStrategy(const char *inputMessage) {
    static char timeBuffer[64];

    if (!strstr(inputMessage, "time")) {
        return NULL;
    }

    time_t now = time(NULL);
    struct tm *localNow = localtime(&now);

    strftime(timeBuffer, sizeof(timeBuffer), "Current time: %H:%M:%S", localNow);

    return timeBuffer;
}
