/*
 * server/src/ai_time.cpp — Time-Query Reply Strategy (C++ / SOLID)
 *
 * SOLID Principles Applied:
 *
 *   Single Responsibility (SRP):
 *     This class has exactly one reason to change: the detection of
 *     time-related queries and the formatting of the current time.
 *     Nothing else lives here.
 *
 *   Liskov Substitution (LSP):
 *     AiTimeStrategy honours the IReplyStrategy contract:
 *       - Returns a formatted time string when "time" appears in input.
 *       - Returns nullptr otherwise.
 *     Fully substitutable for IReplyStrategy* in AiEngine.
 *
 *   Open/Closed (OCP):
 *     Changing the time format or detection keyword requires editing
 *     only this file — AiEngine and all other strategies are untouched.
 *
 * Thread-safety note:
 *   timeBuffer_ is a mutable member buffer. Safe for the current
 *   single-threaded server design. For multi-threading, provide a
 *   caller-owned buffer or use localtime_r().
 */

#include "../../include/model/IReplyStrategy.hpp"

#include <cstring>
#include <ctime>
#include <cstdio>
#include <memory>

namespace ChatBot {

/* ─── AiTimeStrategy ─────────────────────────────────────────────────────── */

class AiTimeStrategy final : public IReplyStrategy {
public:
    /*
     * generateReply — Detect "time" in input and format the current time.
     *
     * Returns: a formatted "Current time: HH:MM:SS" string on match,
     *          nullptr otherwise.
     */
    const char* generateReply(const char* inputMessage) const override {
        if (!std::strstr(inputMessage, "time")) {
            return nullptr;
        }

        std::time_t now      = std::time(nullptr);
        std::tm*    localNow = std::localtime(&now);

        std::strftime(timeBuffer_, sizeof(timeBuffer_),
                      "Current time: %H:%M:%S", localNow);

        return timeBuffer_;
    }

private:
    /*
     * mutable allows this buffer to be written from a const method,
     * preserving the logical const-ness of generateReply().
     */
    mutable char timeBuffer_[64] = {};
};

/*
 * Factory function — lets main.cpp register this strategy via
 * the IReplyStrategy interface without depending on the concrete class.
 */
std::unique_ptr<IReplyStrategy> makeTimeStrategy() {
    return std::make_unique<AiTimeStrategy>();
}

} // namespace ChatBot
