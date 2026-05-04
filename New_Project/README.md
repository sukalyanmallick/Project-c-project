# AI Chatbot — C++ / SOLID / MVC Refactored Project

A Windows TCP chatbot built with **Standard C++17** and Winsock2.
This project has been completely refactored to strictly adhere to the **SOLID principles** and the **Model-View-Controller (MVC)** architectural pattern. 

It has **zero external library dependencies** (no GTK, no GLib), making it highly portable and easy to build on any standard Windows MinGW setup.

---

## Architectural Patterns Applied

### 1. Model-View-Controller (MVC)
The entire codebase is structured into `model/`, `view/`, and `controller/` directories.
- **Model**: Contains the business logic, AI rules (`AiEngine`), and network state. It communicates with other layers purely through callbacks (`IDataReceiver`).
- **View**: Handles all user interface operations (`ConsoleView`) and server logging (`ServerLogger`). It knows nothing about the network.
- **Controller**: The middleman (`ChatController`, `TcpServer`). It intercepts user/network events, modifies the Model, and commands the View to update.

### 2. SOLID Principles
- **S** — Single Responsibility: Each class owns exactly one concern (e.g., `ServerLogger` only logs, `AiTimeStrategy` only replies to time queries).
- **O** — Open/Closed: Add new AI strategies via `engine.addStrategy()` without touching existing files.
- **L** — Liskov Substitution: Concrete strategies substitute perfectly for `IReplyStrategy*`.
- **I** — Interface Segregation: Large network interfaces were split (`IConnectionManager`, `IMessageSender`, `IDataReceiver`).
- **D** — Dependency Inversion: Controllers depend on abstractions, not concrete implementations. 

---

## Project Structure

```
New_Project/
├── shared/
│   └── constants.hpp              ← Typed constexpr constants
│
├── server/
│   ├── Makefile
│   ├── include/
│   │   ├── model/                 ← IReplyStrategy, ai_engine
│   │   ├── view/                  ← ServerLogger
│   │   └── controller/            ← network_server, IServerLifecycle
│   └── src/
│       ├── main.cpp               ← Composition Root (Wires MVC)
│       ├── model/                 ← AI engine & strategies
│       ├── view/                  ← ServerLogger implementation
│       └── controller/            ← TcpServer implementation
│
└── client/
    ├── Makefile
    ├── include/
    │   ├── INetworkClient.hpp     ← ISP Network Interfaces
    │   ├── types.hpp              ← Shared structs
    │   ├── model/                 ← ChatModel, network_client
    │   ├── view/                  ← ConsoleView
    │   └── controller/            ← ChatController
    └── src/
        ├── console_main.cpp       ← Composition Root (Wires MVC)
        ├── model/                 ← ChatModel & TcpNetworkClient implementations
        ├── view/                  ← ConsoleView implementation
        └── controller/            ← ChatController implementation
```

---

## Build Instructions

**Requirements**: A standard Windows MinGW installation (`g++` and `mingw32-make`).

```powershell
# 1. Build the Server
mingw32-make -C server

# 2. Build the Client
mingw32-make -C client console
```

---

## Running the Application

1. Start the server first in your terminal:
   ```powershell
   .\server\chatbot_server.exe
   ```
2. Open a **second terminal window** and start the client:
   ```powershell
   .\client\chatbot_console.exe
   ```
3. Type a message (e.g., "hello", "time", or "how are you") and press **Enter**.
4. Type `bye` to end the session.
