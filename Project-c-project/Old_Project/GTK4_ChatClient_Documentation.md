# 💬 GTK4 Chat Client & AI Chatbot Server — Code Refactoring & Software Engineering Standards Guide

**Course:** Advanced Programming Lab  
**Project:** GTK4 Chat Client + AI Chatbot Server (C Language)  
**Purpose:** Improve code maintainability, readability, modularity, and scalability  
**Date:** February 2026

---

## 📑 Table of Contents

1. [Executive Summary](#executive-summary)
2. [Naming Conventions](#naming-conventions)
3. [Coding Style Guidelines](#coding-style-guidelines)
4. [Folder Structure](#folder-structure)
5. [Modular Design Principles](#modular-design-principles)
6. [Design Patterns for Chat Application](#design-patterns-for-chat-application)
7. [Error Handling & Memory Management](#error-handling--memory-management)
8. [Testing Strategy](#testing-strategy)
9. [Refactoring Roadmap](#refactoring-roadmap)
10. [Future Improvements](#future-improvements)

---

## Executive Summary

### Current Project Overview

The project consists of two components:

**1. GTK4 Chat Client (`client.c`)** — A graphical Windows desktop chat client built with GTK4 and Winsock2. It connects to a local chatbot server, sends user messages, and displays bot replies in real time.

**2. AI Chatbot Server (`server.c`)** — A simple TCP server that listens for a single client connection, receives text messages, and returns keyword-based AI replies.

---

### Strengths

**Client:**
- Functional GTK4 GUI with scrollable chat view
- Real-time message receiving via a dedicated background thread
- Enter key support for sending messages
- Graceful disconnect handling on window close

**Server:**
- Working TCP socket setup with Winsock2
- Simple keyword-matching AI reply generator
- Random fallback response selection
- Correct use of `time()` for time-based replies

---

### Areas for Improvement

**Client:**
- Global `ChatApp app` struct tightly couples all UI and network state
- No error handling on `gtk_text_view_get_buffer()` or thread operations
- Hard-coded `SERVER_IP` and `SERVER_PORT` with no configuration layer
- Missing thread safety safeguards around shared `app.connected` flag
- No graceful shutdown signal to the server on disconnect

**Server:**
- `main()` contains all socket logic — no separation of concerns
- Only handles a single client connection (no loop for multiple clients)
- Uses `strcpy()` instead of safer `strncpy()` alternatives
- `get_ai_reply()` uses static buffers, which are not thread-safe
- No logging or error file output
- Missing input length validation

---

### Refactoring Philosophy

> "Refactor incrementally without breaking the client-server communication."

**Goals:**
- Cleaner, more modular code structure
- Safer memory and string handling
- Better scalability for multiple clients
- Improved readability and professional coding standards

---

## Naming Conventions

### General Principles

- Use descriptive, purpose-driven names
- Avoid single-letter or cryptic abbreviations
- Maintain consistent casing per type (variables, functions, constants, structs)
- Prefix functions with their module name for clarity

---

### Variable Naming Refactoring

#### Before Refactoring
```c
SOCKET sock;
gboolean connected;
char client_message[2000];
char reply[2000];
int r;
int c;
```

**Issues:**
- `r` and `c` give no indication of purpose
- Buffer names don't clarify direction (client → server vs server → client)
- `connected` lacks module context

#### After Refactoring
```c
SOCKET clientSocket;
gboolean isConnected;
char incomingMessage[MAX_BUFFER_SIZE];
char outgoingReply[MAX_BUFFER_SIZE];
int bytesReceived;
int clientAddressLength;
```

**Benefits:**
- Self-documenting variables
- Direction of data flow is immediately clear
- Easier debugging in multi-file projects

---

### Function Naming Refactoring

#### Before
```c
void append_text();
gpointer recv_loop();
void on_connect_clicked();
void on_send_clicked();
const char *get_ai_reply();
```

#### After
```c
void UI_AppendMessage(const char *message);
gpointer Network_ReceiveLoop(gpointer data);
void UI_OnConnectClicked(GtkButton *button, gpointer userData);
void UI_OnSendClicked(GtkButton *button, gpointer userData);
const char* AI_GenerateReply(const char *inputMessage);
```

**Why This Matters:**
- Module prefix (`UI_`, `Network_`, `AI_`) clarifies responsibility
- Verb-noun structure describes action and target
- Consistent with large-scale C project conventions

---

### Constants Naming Refactoring

#### Before
```c
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000
#define BUFFER_SIZE 2048
#define PORT 5000
```

#### After
```c
#define DEFAULT_SERVER_IP     "127.0.0.1"
#define DEFAULT_SERVER_PORT   5000
#define CLIENT_BUFFER_SIZE    2048
#define SERVER_BUFFER_SIZE    2000
#define MAX_GENERIC_REPLIES   5
```

**Improvement:**
- Eliminates ambiguity between client and server constants
- `DEFAULT_` prefix signals these are configurable values
- Descriptive enough to understand usage without context

---

### Structure Naming Refactoring

#### Before
```c
typedef struct {
    GtkWidget *text_view;
    GtkWidget *entry;
    SOCKET sock;
    gboolean connected;
    GThread *recv_thread;
} ChatApp;

ChatApp app = {0};
```

**Issues:**
- Single global instance — hides dependency usage
- No clear distinction between UI fields and network fields
- Hard to unit test or extend to multi-window scenarios

#### After
```c
typedef struct {
    SOCKET      socket;
    gboolean    isConnected;
    GThread    *receiveThread;
} NetworkContext;

typedef struct {
    GtkWidget      *textView;
    GtkWidget      *messageEntry;
    NetworkContext  network;
} ChatApplicationState;
```

**Advantages:**
- Clear separation of UI state and network state
- Easier to pass only the required context to each function
- Supports future multi-instance or multi-window expansion

---

## Coding Style Guidelines

### Indentation

- **4 spaces** preferred
- Avoid mixing tabs and spaces

**Example:**
```c
if (bytesReceived <= 0) {
    Network_Disconnect(&appState.network);
    return NULL;
}
```

### Line Length

- Recommended maximum ≈ **100 characters**
- Break long function signatures logically

**Example:**
```c
// Before (too long)
if (connect(app.sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {

// After (split for clarity)
int connectResult = connect(clientSocket,
                            (struct sockaddr *)&serverAddress,
                            sizeof(serverAddress));
if (connectResult == SOCKET_ERROR) {
```

### Comment Quality Improvement

#### Before
```c
srand(time(NULL)); // Random replies
int r = recv(...); // receive
```

#### After
```c
/*
 * Seed the random number generator once at startup.
 * Used by AI_GenerateReply() to vary generic responses.
 */
srand((unsigned int)time(NULL));

/*
 * Block until data is received from the client.
 * Returns number of bytes received, or SOCKET_ERROR / 0 on disconnect.
 */
int bytesReceived = recv(clientSocket, incomingMessage, SERVER_BUFFER_SIZE - 1, 0);
```

**Principle:**  
Comments should explain **why**, not just **what**.

---

## Folder Structure

### Before Refactoring
```
chat-project/
├── client.c
└── server.c
```

**Problems:**
- Flat structure with no separation
- Impossible to scale
- No header files, build system, or documentation

### Recommended Structure (After Refactoring)
```
chat-project/
├── src/
│   ├── client/
│   │   ├── main.c
│   │   ├── ui.c
│   │   ├── ui.h
│   │   ├── network_client.c
│   │   └── network_client.h
│   ├── server/
│   │   ├── main.c
│   │   ├── ai_engine.c
│   │   ├── ai_engine.h
│   │   ├── network_server.c
│   │   └── network_server.h
│   └── shared/
│       ├── constants.h
│       └── utils.h
├── docs/
├── assets/
├── build/
└── README.md
```

**Benefits:**
- Client and server are independently buildable
- Shared constants (e.g., port number) live in one place
- Professional layout suitable for a portfolio or handoff

---

## Modular Design Principles

### Before Modularization (Server)
```c
int main() {
    // WSAStartup
    // socket()
    // bind()
    // listen()
    // accept()
    // recv/send loop
    // cleanup
}
```

**Problems:**
- All network lifecycle logic packed into `main()`
- No reusable functions
- AI reply logic mixed with socket I/O

### After Modularization (Server)
```c
void Server_Initialize(ServerContext *ctx);
void Server_WaitForClient(ServerContext *ctx);
void Server_HandleClientSession(ServerContext *ctx);
void Server_Shutdown(ServerContext *ctx);
const char* AI_GenerateReply(const char *inputMessage);
```

### After Modularization (Client)
```c
void Network_Connect(NetworkContext *ctx);
void Network_SendMessage(NetworkContext *ctx, const char *message);
gpointer Network_ReceiveLoop(gpointer data);
void Network_Disconnect(NetworkContext *ctx);
void UI_AppendMessage(const char *message);
void UI_BuildLayout(GtkWidget *window, ChatApplicationState *state);
```

### Standard Application Flow
```c
// Server
Server_Initialize(&serverCtx);
Server_WaitForClient(&serverCtx);
Server_HandleClientSession(&serverCtx);
Server_Shutdown(&serverCtx);

// Client (GTK main loop)
while (appRunning) {
    // GTK handles events; callbacks invoke:
    Network_SendMessage();
    Network_ReceiveLoop();  // runs in background thread
    UI_AppendMessage();
}
```

**Advantages:**
- Each function has a single, testable responsibility
- Network context can be reused for multi-client support
- UI layer is fully decoupled from socket logic

---

## Design Patterns for Chat Application

### Thread + Callback Pattern (Current — Improve)

The client already uses a background thread for receiving messages:

```c
app.recv_thread = g_thread_new("recv_thread", recv_loop, NULL);
```

**Improvement:** Pass context explicitly instead of relying on global state:
```c
appState.network.receiveThread = g_thread_new(
    "receive_thread",
    Network_ReceiveLoop,
    &appState  // explicit context, no globals
);
```

### Observer Pattern (Recommended)

**Useful for:**
- Chat message arrival → UI update
- Connection status change → button enable/disable
- Server disconnect event → show reconnect prompt

**Example:**
- `Network_ReceiveLoop` fires → `g_idle_add(UI_AppendMessage_Idle, message)`
- Connection lost → `g_idle_add(UI_ShowDisconnectedState, NULL)`

### Command Pattern (Advanced)

```c
typedef struct {
    char    messageText[CLIENT_BUFFER_SIZE];
    time_t  timestamp;
} SendMessageCommand;
```

**Applications:**
- Message history replay
- Message retry on failed send
- Chat log export feature

### Strategy Pattern (AI Engine)

Replace hardcoded `if/else` chains with a pluggable reply strategy:

```c
typedef const char* (*ReplyStrategy)(const char *input);

const char* AI_KeywordStrategy(const char *input);
const char* AI_RandomStrategy(const char *input);
const char* AI_TimeStrategy(const char *input);
```

**Benefits:**
- Easy to add new AI behaviors
- Testable in isolation
- Extensible to ML-based reply engines in the future

---

## Error Handling & Memory Management

### Safe Socket Handling
```c
SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
if (clientSocket == INVALID_SOCKET) {
    fprintf(stderr, "Socket creation failed. Error: %d\n", WSAGetLastError());
    WSACleanup();
    return FALSE;
}
```

### Safe String Handling (Server)

#### Before
```c
strcpy(reply, response);
```

#### After
```c
strncpy(outgoingReply, response, SERVER_BUFFER_SIZE - 1);
outgoingReply[SERVER_BUFFER_SIZE - 1] = '\0';  // Guarantee null-termination
```

### Thread-Safe Shared State

The `isConnected` flag is read by both the main thread and the receive thread. Protect it:

```c
/*
 * THREAD SAFETY NOTE:
 * isConnected is accessed from both the GTK main thread and the receive thread.
 * Use atomic reads/writes or a mutex to prevent race conditions.
 */
g_atomic_int_set(&appState.network.isConnected, FALSE);
```

### Memory Ownership Documentation
```c
/*
 * Caller is responsible for freeing the returned string via g_free().
 * Returns NULL if message formatting fails.
 */
char* UI_FormatChatMessage(const char *sender, const char *text);
```

### Assertions (Development-Time Validation)
```c
g_assert(appState.network.socket != INVALID_SOCKET);
g_assert(message != NULL);
```

---

## Testing Strategy

### Functional Tests
- Client successfully connects to server on `127.0.0.1:5000`
- Messages typed in the entry are sent and displayed as "You: ..."
- Server replies are displayed as "Bot: ..."
- Typing "bye" disconnects cleanly

### Boundary Tests
- Empty message input (send button pressed with blank entry)
- Message exceeding `CLIENT_BUFFER_SIZE` characters
- Server not running when client attempts to connect
- Rapid consecutive message sends
- Window closed during active receive loop

### AI Reply Tests (Server)
- "hi" / "hello" → greeting response
- "how are you" → status response
- "time" → formatted time string
- Unrecognized input → one of five generic responses (verify randomness)

### Regression Testing
Ensure refactoring does not break:
- GTK4 widget rendering
- Winsock2 connection lifecycle
- Thread startup and cleanup on disconnect

---

## Refactoring Roadmap

### Phase 1 — Cleanup
- Fix naming conventions throughout both files
- Replace global `ChatApp app` with explicit context passing
- Replace `strcpy` with `strncpy` in server
- Improve inline comments to explain intent

### Phase 2 — Modularization
- Split client into `ui.c`, `network_client.c`, and `main.c`
- Split server into `ai_engine.c`, `network_server.c`, and `main.c`
- Create shared `constants.h` for port, buffer sizes, and IP
- Add header files with function declarations

### Phase 3 — Robustness
- Add thread synchronization for `isConnected` flag
- Add input length validation before `send()`
- Add reconnect logic to client on unexpected disconnect
- Enable server to handle multiple clients with a loop or threads

### Phase 4 — Feature Expansion
- Persistent chat history saved to a log file
- Username support in messages
- Server-side message broadcasting (multi-client)
- Configurable server IP/port via GUI or config file

---

## Future Improvements

### Gameplay / Application Features
- Multi-client server support (thread-per-client or `select()`-based)
- Username and session identity
- Emoji or rich-text message support
- Typing indicators and read receipts

### Technical Enhancements
- Cross-platform compatibility (replace Winsock2 with POSIX sockets for Linux/macOS)
- TLS/SSL encrypted connection using OpenSSL
- Configuration file (`config.ini`) for server address and port
- Persistent message log (`.txt` or SQLite)

### Software Engineering Improvements
- Unit testing for `AI_GenerateReply()` using a lightweight C test framework (e.g., Unity)
- CMake or Makefile-based build system
- Continuous integration pipeline (GitHub Actions)
- Automatic code formatting with `clang-format`

---

## Final Note

This refactoring guide aims to:

✅ Improve code quality  
✅ Enhance maintainability  
✅ Support future expansion  
✅ Promote professional coding practices

A well-structured Chat Client and AI Chatbot Server project demonstrates not only functional networking and GUI implementation, but also strong software engineering discipline, modular architecture, and scalability for future enhancements such as multi-user support, encryption, and cross-platform deployment.
