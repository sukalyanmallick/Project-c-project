# AI Chatbot — Refactored Project

A Windows TCP chatbot built with C (Winsock2) on the server side and
GTK4 on the client side. The codebase has been split into focused,
single-purpose modules so each file is easy to read, extend, and test
in isolation.

---

## Project Structure

```
chatBot_refactored/
│
├── shared/
│   └── constants.h              ← Single source of truth for all config values
│
├── server/
│   ├── Makefile
│   ├── include/
│   │   ├── ai_engine.h          ← Strategy type + all strategy + dispatcher declarations
│   │   └── network_server.h     ← ServerContext + 4-phase lifecycle declarations
│   └── src/
│       ├── main.c               ← Entry point: seeds RNG, runs 4 server phases
│       ├── network_server.c     ← Initialize / WaitForClient / HandleSession / Shutdown
│       ├── ai_engine.c          ← Dispatcher: walks strategyTable[], returns first match
│       ├── ai_keyword.c         ← Strategy: greetings, "name", "bye"
│       ├── ai_time.c            ← Strategy: current time when "time" is in input
│       └── ai_random.c          ← Strategy: unconditional random fallback
│
└── client/
    ├── Makefile
    ├── include/
    │   ├── network_client.h     ← NetworkContext + Connect/Send/Recv/Disconnect
    │   └── ui.h                 ← ChatApplicationState + IdleMessageData + all UI decls
    └── src/
        ├── main.c               ← GtkApplication entry point, OnActivate callback
        ├── network_client.c     ← Winsock lifecycle + background receive thread
        ├── ui_display.c         ← UI_AppendMessage + UI_AppendMessage_Idle
        ├── ui_callbacks.c       ← OnConnect / OnSend / OnKeyPressed / OnWindowClose
        └── ui_layout.c          ← UI_BuildLayout: widget tree construction
```

---

## Module Responsibilities

| File | Responsibility |
|---|---|
| `shared/constants.h` | All compile-time constants (IP, port, buffer sizes) |
| `server/src/main.c` | Seeds RNG; runs 4 lifecycle phases in order |
| `server/src/network_server.c` | TCP socket: bind → listen → accept → recv/send → close |
| `server/src/ai_engine.c` | Strategy dispatcher: iterates `strategyTable[]` |
| `server/src/ai_keyword.c` | Keyword matching (hello, bye, name, how are you) |
| `server/src/ai_time.c` | Replies with current time when "time" is detected |
| `server/src/ai_random.c` | Random fallback from a pool of generic replies |
| `client/src/main.c` | GtkApplication; allocates state; hands off to GTK loop |
| `client/src/network_client.c` | Winsock connect/send/recv-thread/disconnect |
| `client/src/ui_display.c` | Writes to GtkTextView; idle-safe cross-thread wrapper |
| `client/src/ui_callbacks.c` | All four GTK signal callbacks |
| `client/src/ui_layout.c` | Builds widget hierarchy; wires signals; shows welcome text |

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

## Extending the AI

To add a new reply category:

1. Create `server/src/ai_<category>.c` with a function matching
   the `ReplyStrategy` signature (`const char *fn(const char *)`).
2. Declare it in `server/include/ai_engine.h`.
3. Add it to `strategyTable[]` in `server/src/ai_engine.c`.
4. Add the new `.c` file to `SRCS` in `server/Makefile`.

No other files need to change.

---

## Key Design Decisions

- **No global variables** — all state flows through `ServerContext`
  (server) and `ChatApplicationState` (client).
- **Atomic flag** — `NetworkContext.isConnected` is a `gint` read and
  written only via `g_atomic_int_get/set` to prevent races between the
  GTK main thread and the background receive thread.
- **g_idle_add() bridge** — the receive thread never calls GTK
  functions directly; it posts `IdleMessageData` payloads to the main
  loop, which then calls `UI_AppendMessage_Idle`.
- **Strategy table** — adding a new AI category is a 4-step procedure
  that never requires editing existing strategy files.
