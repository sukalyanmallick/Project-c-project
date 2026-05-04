/*
 * server/include/IReplyStrategy.hpp — AI Reply Strategy Interface
 *
 * SOLID Principles Applied:
 *
 *   Open/Closed Principle (OCP):
 *     The AI engine is closed for modification but open for extension.
 *     To add a new reply category, implement this interface — no existing
 *     file needs to change.
 *
 *   Dependency Inversion Principle (DIP):
 *     AiEngine depends on this abstraction, never on concrete strategy
 *     classes. High-level policy (dispatch) is decoupled from low-level
 *     detail (keyword matching, time lookup, etc.).
 *
 *   Liskov Substitution Principle (LSP):
 *     Every concrete IReplyStrategy must honour the contract:
 *       - Return a valid const char* when it handles the input.
 *       - Return nullptr when it cannot handle the input.
 *     Any concrete strategy can be substituted for IReplyStrategy* without
 *     changing AiEngine's behaviour.
 *
 *   Interface Segregation Principle (ISP):
 *     This interface is intentionally minimal — one pure virtual method.
 *     Concrete strategies are not forced to implement unrelated methods.
 *
 * Ownership:
 *   Returned strings point to storage managed inside the strategy
 *   implementation. Callers must NOT free them.
 */

#ifndef IREPLY_STRATEGY_HPP
#define IREPLY_STRATEGY_HPP

namespace ChatBot {

class IReplyStrategy {
public:
    /*
     * generateReply — Inspect inputMessage and produce a reply.
     *
     * Parameters:
     *   inputMessage — null-terminated string from the client. Must not be nullptr.
     *
     * Returns:
     *   A null-terminated reply string on match, nullptr if this strategy
     *   cannot handle the input.
     *
     * Ownership:
     *   The returned pointer is valid until the next call on this instance.
     *   Callers must NOT free it.
     */
    virtual const char* generateReply(const char* inputMessage) const = 0;

    /*
     * Virtual destructor ensures derived objects are properly destroyed
     * when held as IReplyStrategy*.
     */
    virtual ~IReplyStrategy() = default;
};

} // namespace ChatBot

#endif /* IREPLY_STRATEGY_HPP */
