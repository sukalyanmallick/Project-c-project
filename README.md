# Windows Event Viewer — GTK4 Implementation

**Project:** Windows Event Viewer Refactoring Journey
**Course:** Advanced Programming Laboratory (CSE 2100)
**Students:** Sukalyan & Mizanur
**Session:** 0714-02
**Date:** February 2025

---

## Table of Contents

**Part A — Project Overview** ✅
1. [Introduction](#1-introduction)
2. [Three Project Versions](#2-three-project-versions)
3. [Key Features](#3-key-features)
4. [Technology Stack](#4-technology-stack)
5. [Repository Structure](#5-repository-structure)

**Part B — Build & Usage** ✅
6. [Build Requirements](#6-build-requirements)
7. [Compilation Instructions](#7-compilation-instructions)
8. [Running the Application](#8-running-the-application)
9. [Troubleshooting](#9-troubleshooting)

**Part C — Architecture & Design** ✅
10. [Folder Structure](#10-folder-structure)
11. [Version Comparison](#11-version-comparison)
12. [Architecture Differences](#12-architecture-differences)

**Part D — Refactoring Documentation** ✅
13. [AI Prompts Used](#13-ai-prompts-used)
14. [SOLID Principles Analysis](#14-solid-principles-analysis)
15. [Design Patterns Implemented](#15-design-patterns-implemented)
16. [Detailed Change Log](#16-detailed-change-log)
17. [Lessons Learned](#17-lessons-learned)

**Part E — Appendices** ✅
18. [File Metrics Comparison](#18-file-metrics-comparison)
19. [Dependency Flow Verification](#19-dependency-flow-verification)
20. [Contact & References](#20-contact--references)

---

## Part A — Project Overview

### 1. Introduction

**Chatbot** is a GTK4-based Windows chat application that connects a GUI client to an AI-powered server over TCP/IP. The project was developed as part of the Advanced Programming Laboratory course to demonstrate software refactoring, SOLID principles, and clean architecture in a real C application.

The project evolved through three versions — from a monolithic C implementation to a fully refactored, SOLID-compliant C++ architecture.

---

### 2. Three Project Versions

| Version | Language | Description |
|---------|----------|-------------|
| **v1 — Original** | C | Single-file monolithic implementation. All logic in one place. |
| **v2 — Refactored C** | C | Multi-file separation. Client/server split. Shared constants. |
| **v3 — SOLID C** | C | SOLID principles applied. Strategy pattern. Dependency injection. |

---

### 3. Key Features

- **GTK4 GUI Client** — Chat window with connect, send, and disconnect controls
- **AI Reply Engine** — Strategy-based response system (keyword, time, random fallback)
- **TCP/IP Networking** — Winsock2-based client-server communication
- **Threaded Receive Loop** — Non-blocking background receive thread via GThread
- **SOLID Architecture** — All five principles applied across server and client
- **Extensible AI** — Add new keyword replies by editing a data table only

---

### 4. Technology Stack

| Component | Technology |
|-----------|-----------|
| GUI Framework | GTK4 (via GLib/GObject) |
| Networking | Winsock2 (ws2_32) |
| Language | C11 |
| Build System | GNU Make (MinGW-w64) |
| Threading | GThread (GLib) |
| Platform | Windows |

---

### 5. Repository Structure

```
chatBot_SOLID/
├── shared/
│   └── constants.h              # Shared constants (IP, ports, buffer sizes)
├── server/
│   ├── include/
│   │   ├── ai_engine.h          # AI strategy interface
│   │   ├── ai_keyword.h         # KeywordRule struct (OCP)
│   │   ├── network_server.h     # Server lifecycle interface
│   │   └── session_handler.h    # MessageHandlerFn + exit logic (DIP/SRP)
│   ├── src/
│   │   ├── main.c               # Composition root — injects AI handler
│   │   ├── network_server.c     # TCP lifecycle (bind/listen/accept/send/recv)
│   │   ├── session_handler.c    # Session exit condition (SRP)
│   │   ├── ai_engine.c          # Strategy dispatcher
│   │   ├── ai_keyword.c         # Keyword table strategy (OCP)
│   │   ├── ai_time.c            # Time query strategy
│   │   └── ai_random.c          # Random fallback strategy
│   └── Makefile
├── client/
│   ├── include/
│   │   ├── ui.h                 # UI state + ISP view types
│   │   └── network_client.h     # Client network interface
│   ├── src/
│   │   ├── main.c               # GTK application entry point
│   │   ├── ui_layout.c          # Widget construction
│   │   ├── ui_callbacks.c       # GTK signal handlers
│   │   ├── ui_display.c         # Chat history display
│   │   └── network_client.c     # Winsock2 TCP client
│   └── Makefile
├── SOLID_CHANGES.md             # Refactoring notes
└── Makefile                     # Root build (builds both server and client)
```

---

## Part B — Build & Usage

### 6. Build Requirements

- **OS:** Windows 10/11
- **Compiler:** GCC via MinGW-w64 (MSYS2 recommended)
- **GTK4:** Installed and on PATH (via MSYS2 `mingw-w64-x86_64-gtk4`)
- **Make:** GNU Make (`mingw32-make` or `make` in MSYS2)
- **Winsock2:** Included with Windows SDK (linked via `-lws2_32`)

Install dependencies via MSYS2:
```bash
pacman -S mingw-w64-x86_64-gtk4 mingw-w64-x86_64-gcc make
```

---

### 7. Compilation Instructions

**Build everything (server + client):**
```bash
make
```

**Build server only:**
```bash
cd server
make
```

**Build client only:**
```bash
cd client
make
```

**Clean build artifacts:**
```bash
make clean
```

---

### 8. Running the Application

**Step 1 — Start the server:**
```bash
./server/chatbot_server.exe
```
Expected output:
```
[Server] AI Chatbot Server starting...
[Server] Listening on port 5000...
[Server] Waiting for a client...
```

**Step 2 — Launch the client:**
```bash
./client/chatbot_client.exe
```

**Step 3 — Connect and chat:**
1. Click **Connect** in the GUI
2. Type a message and press **Enter** or click **Send**
3. Type `bye` to disconnect

---

### 9. Troubleshooting

| Problem | Cause | Fix |
|---------|-------|-----|
| `WSAStartup failed` | Winsock DLL missing | Ensure ws2_32.dll is on PATH |
| `Connection failed` | Server not running | Start the server before the client |
| GTK window doesn't open | GTK4 DLLs missing | Add MSYS2 mingw64/bin to PATH |
| `bind() failed` | Port 5000 in use | Change `DEFAULT_SERVER_PORT` in `shared/constants.h` |
| `socket() failed` | Firewall blocking | Allow the application through Windows Firewall |

---

## Part C — Architecture & Design

### 10. Folder Structure

The project uses a **monorepo layout** with three top-level directories:

- `shared/` — constants visible to both server and client
- `server/` — AI engine + TCP server (standalone executable)
- `client/` — GTK4 GUI + TCP client (standalone executable)

Each of `server/` and `client/` follows the same internal structure: `include/` for headers, `src/` for implementations, and a `Makefile` for building.

---

### 11. Version Comparison

| Aspect | v1 (Original) | v2 (Refactored C) | v3 (SOLID) |
|--------|---------------|-------------------|------------|
| Files | 1–2 | ~8 | ~14 |
| Global variables | Many | None | None |
| AI extensibility | Edit if-chain | Edit if-chain | Append table row |
| AI/Network coupling | Tight | Tight | Decoupled via DIP |
| Exit logic location | Inside session loop | Inside session loop | `session_handler.c` |
| Callback dependencies | Full state | Full state | Narrow view types (ISP) |

---

### 12. Architecture Differences

**v1 → v2:** Extracted monolithic code into separate files with clear module boundaries. Eliminated global variables using `ServerContext` and `ChatApplicationState`.

**v2 → v3 (SOLID):** Five targeted changes, one per principle — see Part D.

---

## Part D — Refactoring Documentation

### 13. AI Prompts Used

The following AI-assisted prompts guided the refactoring process:

1. *"Refactor this C chatbot into client/server with no global variables"*
2. *"Apply SOLID principles to this C project — identify violations first, then fix each one"*
3. *"Replace the if-chain in ai_keyword.c with a data-driven approach that satisfies OCP"*
4. *"How do I do dependency injection in C without classes?"*
5. *"Split ChatApplicationState so GTK callbacks only see the fields they need (ISP)"*

---

### 14. SOLID Principles Analysis

#### S — Single Responsibility
**Violation:** `Server_HandleClientSession()` handled recv/send, AI calls, and session exit logic in one function.
**Fix:** Extracted `Session_ShouldExit()` into `session_handler.c`. Each unit now has one reason to change.

#### O — Open/Closed
**Violation:** `AI_KeywordStrategy()` used a hardcoded `if/else` chain. Adding keywords required modifying existing code.
**Fix:** Replaced with a `KeywordRule[]` data table. New keywords are added by appending rows — existing code is closed for modification.

#### L — Liskov Substitution
**Already satisfied.** All AI strategies conform to the `ReplyStrategy` function pointer type and are interchangeable in the dispatch table without changing caller behavior.

#### I — Interface Segregation
**Violation:** All GTK callbacks received `ChatApplicationState*` (both widgets and socket). Network-only callbacks were forced to depend on widget fields they never used.
**Fix:** Added `NetworkView` and `DisplayView` as narrow types in `ui.h`. Callbacks depend only on the interface they actually use.

#### D — Dependency Inversion
**Violation:** `network_server.c` (low-level) called `AI_GenerateReply()` (high-level) directly — coupling mechanism to policy.
**Fix:** Introduced `MessageHandlerFn` (a function pointer typedef). `Server_HandleClientSession()` accepts it as a parameter. `main()` injects `AI_GenerateReply` at the composition root.

---

### 15. Design Patterns Implemented

| Pattern | Location | Description |
|---------|----------|-------------|
| **Strategy** | `server/src/ai_engine.c` | `strategyTable[]` dispatches through `ReplyStrategy` function pointers |
| **Dependency Injection** | `server/src/main.c` | `AI_GenerateReply` injected into `Server_HandleClientSession()` |
| **Observer (via idle)** | `client/src/network_client.c` | Background thread posts to GTK main thread via `g_idle_add()` |
| **Facade** | `server/include/network_server.h` | Four-phase lifecycle hides all Winsock complexity |

---

### 16. Detailed Change Log

| File | Change | Principle |
|------|--------|-----------|
| `server/include/session_handler.h` | **NEW** — `MessageHandlerFn` typedef + `Session_ShouldExit` declaration | SRP, DIP |
| `server/src/session_handler.c` | **NEW** — exit condition extracted from session loop | SRP |
| `server/include/ai_keyword.h` | **NEW** — `KeywordRule` struct definition | OCP |
| `server/src/ai_keyword.c` | **MODIFIED** — if-chain replaced with `keywordRules[]` table | OCP |
| `server/include/network_server.h` | **MODIFIED** — `Server_HandleClientSession` accepts `MessageHandlerFn` | DIP |
| `server/src/network_server.c` | **MODIFIED** — calls handler parameter, delegates exit to `Session_ShouldExit` | SRP, DIP |
| `server/src/main.c` | **MODIFIED** — injects `AI_GenerateReply` as composition root | DIP |
| `client/include/ui.h` | **MODIFIED** — added `NetworkView` and `DisplayView` narrow types | ISP |

---

### 17. Lessons Learned

- **DIP in C is done with function pointers.** Without classes, `typedef`-ing a function pointer is the idiomatic way to define an abstraction boundary.
- **OCP is often a data problem.** Many if-chains are really just hardcoded tables. Moving data out of logic makes code open for extension by default.
- **SRP violations are easy to spot:** if you need "and" to describe what a function does, it has more than one responsibility.
- **ISP in C is structural.** You can't use interface types, but you can define narrow structs that expose only relevant fields to each consumer.
- **The composition root is everything in DIP.** The injector (`main.c`) is the only place that knows about both the high-level policy and the low-level mechanism.

---

## Part E — Appendices

### 18. File Metrics Comparison

| File | v2 Lines | v3 Lines | Delta |
|------|----------|----------|-------|
| `network_server.c` | ~130 | ~120 | -10 (exit logic extracted) |
| `ai_keyword.c` | ~35 | ~30 | -5 (if-chain → table) |
| `main.c` (server) | ~40 | ~38 | ~same, cleaner injection |
| `session_handler.c` | — | ~20 | NEW |
| `ai_keyword.h` | — | ~20 | NEW |
| `session_handler.h` | — | ~35 | NEW |
| **Total** | ~350 | ~400 | +50 (better separation) |

---

### 19. Dependency Flow Verification

```
main.c (composition root)
  ├── injects AI_GenerateReply → Server_HandleClientSession()
  ├── calls Server_Initialize()
  ├── calls Server_WaitForClient()
  └── calls Server_HandleClientSession(ctx, handler)
            ├── handler(msg)          ← AI dependency via abstraction
            └── Session_ShouldExit()  ← SRP-extracted exit check

ai_engine.c
  └── strategyTable[]
        ├── AI_KeywordStrategy()  ← keywordRules[] data table (OCP)
        ├── AI_TimeStrategy()
        └── AI_RandomStrategy()   ← guaranteed fallback

client/ui.h
  ├── ChatApplicationState (full state — main + layout)
  ├── NetworkView           (network-only callbacks)
  └── DisplayView           (display-only callbacks)
```

**Key:** `network_server.c` does NOT include `ai_engine.h` — the DIP boundary is enforced at the include level.

---

### 20. Contact & References

**Students:**
- Sukalyan — Advanced Programming Laboratory, CSE 2100
- Mizanur — Advanced Programming Laboratory, CSE 2100

**Session:** 0714-02 | **Date:** February 2025

**References:**
- [GTK4 Documentation](https://docs.gtk.org/gtk4/)
- [Winsock2 Reference — Microsoft Docs](https://learn.microsoft.com/en-us/windows/win32/winsock/winsock-reference)
- Robert C. Martin — *Clean Architecture*
- Robert C. Martin — *Agile Software Development: Principles, Patterns, and Practices*
- [GLib Threading — GThread](https://docs.gtk.org/glib/struct.Thread.html)

---

*Generated as part of the CSE 2100 Advanced Programming Laboratory refactoring assignment.*
