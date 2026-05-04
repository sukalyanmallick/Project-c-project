/*
 * client/include/ui.hpp — GTK4 Chat Client UI Layer Interface (C++ / SOLID)
 *
 * SOLID Principles Applied:
 *
 *   Single Responsibility (SRP):
 *     This header owns only the UI layer contract: widget state, the
 *     cross-thread idle payload, and the GTK signal callbacks. Network
 *     details are encapsulated inside TcpNetworkClient.
 *
 *   Dependency Inversion (DIP):
 *     ChatApplicationState holds IConnectionManager* and IMessageSender*
 *     interfaces, not a concrete TcpNetworkClient. This lets the UI layer
 *     work with any conforming network implementation (real or mock).
 *
 *   Interface Segregation (ISP):
 *     UI callbacks that only send messages depend on IMessageSender*.
 *     Callbacks that connect/disconnect depend on IConnectionManager*.
 *     Neither group needs to know about the other's interface.
 *
 * Note on GTK4 signal callbacks:
 *   GTK4 signal callbacks must be plain functions (or compatible with
 *   the GCallback / GClosure mechanism). They are therefore kept as
 *   free functions rather than class methods, receiving the full
 *   ChatApplicationState* through the userData / gpointer parameter.
 */

#ifndef UI_HPP
#define UI_HPP

#include <gtk/gtk.h>
#include "INetworkClient.hpp"

namespace ChatBot {

/* ─── ChatApplicationState ───────────────────────────────────────────────── */

/*
 * Central state struct passed to every GTK signal callback via user_data.
 * Eliminates all global variables by carrying both widget handles and
 * network interfaces in a single heap-allocated object.
 *
 * DIP: holds abstractions (IConnectionManager*, IMessageSender*),
 *      not the concrete TcpNetworkClient.
 */
struct ChatApplicationState {
    GtkWidget*          textView;       /* Read-only scrollable chat history  */
    GtkWidget*          messageEntry;   /* Single-line message input field    */
    IConnectionManager* connector;      /* Owns connect / disconnect          */
    IMessageSender*     sender;         /* Owns sendMessage                   */
};

/* ─── IdleMessageData ────────────────────────────────────────────────────── */

/*
 * Payload carried through g_idle_add() from the receive thread to the
 * GTK main thread. The idle callback (UI_AppendMessage_Idle) owns this
 * struct and frees both the struct and its message field after use.
 */
struct IdleMessageData {
    ChatApplicationState* appState;
    char*                 message; /* Heap-allocated; freed by idle callback */
};

/* ─── Layout Construction ────────────────────────────────────────────────── */

/*
 * UI_BuildLayout — Build and attach all GTK4 widgets to the window.
 *
 * Widget hierarchy created:
 *   GtkApplicationWindow
 *   └── GtkBox (vertical)
 *       ├── GtkScrolledWindow → GtkTextView  (chat history)
 *       ├── GtkEntry                          (message input)
 *       └── GtkBox (horizontal)
 *           ├── GtkButton "Connect"
 *           └── GtkButton "Send"
 */
void UI_BuildLayout(GtkWidget* window, ChatApplicationState* appState);

/* ─── Message Display ────────────────────────────────────────────────────── */

/* UI_AppendMessage — Append text + newline to the chat view (main thread only). */
void UI_AppendMessage(ChatApplicationState* appState, const char* message);

/* UI_AppendMessage_Idle — g_idle_add()-compatible cross-thread wrapper. */
gboolean UI_AppendMessage_Idle(gpointer data);

/* ─── Signal Callbacks ───────────────────────────────────────────────────── */

void     UI_OnConnectClicked    (GtkButton* button, gpointer userData);
void     UI_OnSendClicked       (GtkButton* button, gpointer userData);
gboolean UI_OnEntryKeyPressed   (GtkEventControllerKey* controller,
                                  guint keyval,
                                  guint keycode,
                                  GdkModifierType state,
                                  gpointer userData);
gboolean UI_OnWindowClose       (GtkWindow* window, gpointer userData);

} // namespace ChatBot

#endif /* UI_HPP */
