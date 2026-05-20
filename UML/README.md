# UML Diagrams — AI Chatbot Project

## Class Diagram

### Interfaces
| Interface | Method |
|---|---|
| IReplyStrategy | getReply(msg): string |
| IServerLifecycle | start(), stop() |
| INetworkClient | connect(), send(), receive() |

### Server Classes (MVC)
| Class | Layer | Role |
|---|---|---|
| NetworkServer | Controller | Client connection handle |
| AIEngine | Model | Reply strategy select |
| AIKeyword | Model | Keyword reply |
| AITime | Model | Time based reply |
| AIRandom | Model | Random reply |
| ServerLogger | View | Log display |

### Client Classes (MVC)
| Class | Layer | Role |
|---|---|---|
| ChatController | Controller | Input coordinator |
| ChatModel | Model | Message history |
| ConsoleView | View | Terminal display |
| NetworkClient | Network | TCP socket |

## MVC Flow
User types message
→ ChatController
→ ChatModel (save)
→ ConsoleView (display)
→ NetworkClient (send)
→ NetworkServer (receive)
→ AIEngine (reply)
→ ServerLogger (log)
→ Client reply

## SOLID Principles
- S: Each class has one responsibility
- O: New AI strategy without changing old code
- L: Any strategy replaceable via IReplyStrategy
- I: Small separate interfaces
- D: Dependency injection via interfaces
