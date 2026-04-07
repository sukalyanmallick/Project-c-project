/*
 * client/include/ui.h — GTK4 Chat Client UI Layer Interface
 *
 * Declares:
 *   - ChatApplicationState  : all widget pointers + embedded NetworkContext
 *   - IdleMessageData       : cross-thread message payload
 *   - UI_BuildLayout        : widget construction
 *   - UI_AppendMessage*     : chat history display
 *   - UI_On*                : GTK signal callbacks
 *
 * Design principle:
 *   The UI layer owns all GTK widgets and handles all user interactions.
 *   It delegates network operations to network_client.h and never
 *   touches Winsock APIs directly.
 */

#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>
#include "network_client.h"

/* ─── ChatApplicationState ───────────────────────────────────────────────── */

/*
 * Central state struct passed to every GTK signal callback via user_data.
 * Eliminates all global variables by carrying both widget handles and
 * network context in a single, heap-allocated object.
 */
typedef struct ChatApplicationState {
    GtkWidget      *textView;       /* Read-only scrollable chat history    */
    GtkWidget      *messageEntry;   /* Single-line message input field      */
    NetworkContext  network;        /* Socket + atomic flag + recv thread   */
} ChatApplicationState;

/* ─── IdleMessageData ────────────────────────────────────────────────────── */

/*
 * Payload carried through g_idle_add() from the receive thread to the
 * GTK main thread. The idle callback (UI_AppendMessage_Idle) owns this
 * struct and frees both the struct and its message field after use.
 */
typedef struct {
    ChatApplicationState *appState;
    char                 *message;  /* Heap-allocated; freed by idle callback */
} IdleMessageData;

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
 *
 * Parameters:
 *   window   — The top-level GtkApplicationWindow.
 *   appState — Application state forwarded to all signal callbacks.
 */
void UI_BuildLayout(GtkWidget *window, ChatApplicationState *appState);

/* ─── Message Display ────────────────────────────────────────────────────── */

/*
 * UI_AppendMessage — Append text + newline to the chat view.
 *
 * Must be called from the GTK main thread only.
 * For cross-thread use, call UI_AppendMessage_Idle via g_idle_add().
 */
void UI_AppendMessage(ChatApplicationState *appState, const char *message);

/*
 * UI_AppendMessage_Idle — g_idle_add()-compatible wrapper.
 *
 * Takes ownership of the IdleMessageData* passed as `data`, calls
 * UI_AppendMessage(), then frees the payload. Returns G_SOURCE_REMOVE
 * so the source fires exactly once.
 */
gboolean UI_AppendMessage_Idle(gpointer data);

/* ─── Signal Callbacks ───────────────────────────────────────────────────── */

/*
 * UI_OnConnectClicked — "clicked" callback for the Connect button.
 * Delegates to Network_Connect().
 */
void UI_OnConnectClicked(GtkButton *button, gpointer userData);

/*
 * UI_OnSendClicked — "clicked" callback for the Send button.
 * Reads the entry, calls Network_SendMessage(), echoes to the view,
 * and clears the entry.
 */
void UI_OnSendClicked(GtkButton *button, gpointer userData);

/*
 * UI_OnEntryKeyPressed — "key-pressed" callback for the message entry.
 * Triggers UI_OnSendClicked when GDK_KEY_Return is detected.
 */
gboolean UI_OnEntryKeyPressed(GtkEventControllerKey *controller,
                               guint keyval,
                               guint keycode,
                               GdkModifierType state,
                               gpointer userData);

/*
 * UI_OnWindowClose — "close-request" callback for the top-level window.
 * Disconnects the network and joins the receive thread before the
 * window (and its widgets) are destroyed.
 */
gboolean UI_OnWindowClose(GtkWindow *window, gpointer userData);

#endif /* UI_H */
