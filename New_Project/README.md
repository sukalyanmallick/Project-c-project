# AI Chatbot — C++ / SOLID Refactored Project

A Windows TCP chatbot built with **C++17** (Winsock2) on the server side and
**GTK4** on the client side. All source files have been converted from C to C++
and restructured to apply all five **SOLID principles** throughout.

---

## SOLID Principles Applied

| Principle | Where |
|---|---|
| **S** — Single Responsibility | Each class/file owns exactly one concern |
| **O** — Open/Closed | Add strategies via `addStrategy()` — existing code untouched |
| **L** — Liskov Substitution | Every concrete strategy is safely substitutable for `IReplyStrategy*` |
| **I** — Interface Segregation | `IConnectionManager` + `IMessageSender` replace one fat network struct |
| **D** — Dependency Inversion | `AiEngine` depends on `IReplyStrategy*`; UI depends on `IConnectionManager*` / `IMessageSender*` |

---

## Project Structure

```
New_Project/
│
├── shared/
│   └── constants.hpp              ← Typed constexpr constants in namespace ChatBot
│
├── server/
│   ├── Makefile                   ← g++ / C++17
│   ├── include/
│   │   ├── IReplyStrategy.hpp     ← [NEW] Pure abstract interface (OCP / DIP)
│   │   ├── IServerLifecycle.hpp   ← [NEW] Four-phase lifecycle interface (ISP / OCP)
│   │   ├── ai_engine.hpp          ← AiEngine class (SRP / OCP / DIP)
│   │   └── network_server.hpp     ← TcpServer class (SRP / DIP)
│   └── src/
│       ├── main.cpp               ← Composition Root: wires strategies → engine → server
│       ├── network_server.cpp     ← TcpServer: implements IServerLifecycle
│       ├── ai_engine.cpp          ← AiEngine: dispatch loop over IReplyStrategy*
│       ├── ai_keyword.cpp         ← AiKeywordStrategy: implements IReplyStrategy (SRP/LSP)
│       ├── ai_time.cpp            ← AiTimeStrategy:    implements IReplyStrategy (SRP/LSP)
│       └── ai_random.cpp          ← AiRandomStrategy:  implements IReplyStrategy (SRP/LSP)
│
└── client/
    ├── Makefile                   ← g++ / C++17
    ├── include/
    │   ├── INetworkClient.hpp     ← [NEW] IConnectionManager + IMessageSender (ISP)
    │   ├── network_client.hpp     ← TcpNetworkClient class
    │   └── ui.hpp                 ← ChatApplicationState (with ISP interfaces), callbacks
    └── src/
        ├── main.cpp               ← Composition Root: creates TcpNetworkClient, injects interfaces
        ├── network_client.cpp     ← TcpNetworkClient: implements both ISP interfaces
        ├── ui_display.cpp         ← UI_AppendMessage + Idle wrapper (SRP)
        ├── ui_callbacks.cpp       ← GTK signal callbacks via IConnectionManager* / IMessageSender*
        └── ui_layout.cpp          ← Widget tree construction (SRP)
```

---

## Module Responsibilities

| File | Responsibility |
|---|---|
| `shared/constants.hpp` | All `constexpr` config values in `namespace ChatBot` |
| `server/include/IReplyStrategy.hpp` | Pure abstract strategy interface (OCP/DIP) |
| `server/include/IServerLifecycle.hpp` | Four-phase lifecycle interface (ISP/OCP) |
| `server/include/ai_engine.hpp` | `AiEngine` class: owns `vector<unique_ptr<IReplyStrategy>>` |
| `server/include/network_server.hpp` | `TcpServer` class: implements `IServerLifecycle` |
| `server/src/main.cpp` | Composition Root: seeds RNG, builds engine + strategies + server |
| `server/src/network_server.cpp` | Winsock2 TCP lifecycle; delegates AI to injected `AiEngine&` |
| `server/src/ai_engine.cpp` | Walks strategy vector, returns first non-null reply |
| `server/src/ai_keyword.cpp` | `AiKeywordStrategy`: greetings, name, bye |
| `server/src/ai_time.cpp` | `AiTimeStrategy`: replies with current time |
| `server/src/ai_random.cpp` | `AiRandomStrategy`: unconditional random fallback |
| `client/include/INetworkClient.hpp` | `IConnectionManager` + `IMessageSender` (ISP split) |
| `client/include/network_client.hpp` | `TcpNetworkClient`: implements both ISP interfaces |
| `client/include/ui.hpp` | `ChatApplicationState` (holds interface pointers), callbacks |
| `client/src/main.cpp` | Composition Root: creates `TcpNetworkClient`, injects as interfaces |
| `client/src/network_client.cpp` | Winsock connect/send/recv-thread/disconnect |
| `client/src/ui_display.cpp` | `UI_AppendMessage` + `UI_AppendMessage_Idle` |
| `client/src/ui_callbacks.cpp` | GTK signal callbacks; access only `IConnectionManager*` / `IMessageSender*` |
| `client/src/ui_layout.cpp` | Builds widget hierarchy; wires signals |

---

## Build

### Prerequisites (MSYS2 / MinGW-w64)

```bash
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-gtk4
pacman -S mingw-w64-x86_64-pkg-config
```

### Build both

```bash
make
```

### Build individually

```bash
make server    # → server/chatbot_server.exe
make client    # → client/chatbot_client.exe
```

### Clean

```bash
make clean
```

---

## Running

1. Start the server first:
   ```
   server/chatbot_server.exe
   ```
2. Launch the client:
   ```
   client/chatbot_client.exe
   ```
3. Click **Connect** in the client window.
4. Type a message and press **Enter** or click **Send**.
5. Type `bye` to end the session.

---

## Extending the AI (OCP in action)

To add a new reply category — **zero existing files change**:

1. Create `server/src/ai_<category>.cpp` implementing `IReplyStrategy`:
   ```cpp
   class AiWeatherStrategy final : public ChatBot::IReplyStrategy {
   public:
       const char* generateReply(const char* input) const override {
           if (std::strstr(input, "weather")) return "It's always sunny in C++!";
           return nullptr;
       }
   };
   std::unique_ptr<ChatBot::IReplyStrategy> makeWeatherStrategy() {
       return std::make_unique<AiWeatherStrategy>();
   }
   ```
2. Declare `makeWeatherStrategy()` in `server/src/main.cpp`.
3. Call `engine.addStrategy(makeWeatherStrategy());` in `main.cpp`.
4. Add `src/ai_weather.cpp` to `SRCS` in `server/Makefile`.

No other files change.

---

## Key Design Decisions

- **Composition Root** — `main.cpp` in both server and client is the *only* place that names concrete types. All other code works through interfaces.
- **`std::unique_ptr` ownership** — `AiEngine` owns strategies via `unique_ptr`; no manual `delete`.
- **Atomic flag** — `TcpNetworkClient::connectedFlag_` is a `gint` read/written only via `g_atomic_int_get/set` to prevent data races.
- **`g_idle_add()` bridge** — The receive thread never calls GTK functions directly; it posts `IdleMessageData` payloads to the main loop.
- **Factory functions** — Each strategy `.cpp` exports a `make*Strategy()` factory so `main.cpp` registers them through the `IReplyStrategy` interface (DIP preserved even at the registration site).
- **`constexpr` over `#define`** — `constants.hpp` uses typed `constexpr` values inside `namespace ChatBot` instead of preprocessor macros, eliminating accidental name collisions.
