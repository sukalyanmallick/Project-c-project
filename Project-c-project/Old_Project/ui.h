/*
 * ui.h — GTK4 Chat Client UI Layer Interface
 *
 * Declares the ChatApplicationState struct and all UI functions.
 * The UI layer is fully decoupled from socket logic: it calls into
 * network_client.h for all network operations but never touches
 * Winsock directly.
 */

#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>
#include "network_client.h"

/* ─── ChatApplicationState ───────────────────────────────────────────────── */

/*
 * ChatApplicationState holds all widget pointers and the embedded network
 * context. Passing a pointer to this struct into every callback removes
 * the need for global variables.
 */
struct ChatApplicationState {
    GtkWidget      *textView;       /* Scrollable chat history display          */
    GtkWidget      *messageEntry;   /* Single-line text input field             */
    NetworkContext  network;        /* Winsock socket + thread state            */
};

/* ─── UI Lifecycle ───────────────────────────────────────────────────────── */

/*
 * UI_BuildLayout — Construct and attach all GTK4 widgets to the window.
 *
 * Creates the scrolled text view, message entry, Connect button, and
 * Send button. Connects all signal handlers with appState as user_data.
 *
 * Parameters:
 *   window   — The GtkApplicationWindow to populate.
 *   appState — Application state passed through to all signal callbacks.
 */
void UI_BuildLayout(GtkWidget *window, struct ChatApplicationState *appState);

/* ─── UI Message Display ─────────────────────────────────────────────────── */

/*
 * UI_AppendMessage — Append a line of text to the chat history view.
 *
 * Must be called from the GTK main thread. For cross-thread use,
 * call UI_AppendMessage_Idle() via g_idle_add() instead.
 *
 * Parameters:
 *   appState — State containing the textView widget.
 *   message  — Null-terminated string to append. Must not be NULL.
 */
void UI_AppendMessage(struct ChatApplicationState *appState, const char *message);

/*
 * UI_AppendMessage_Idle — g_idle_add() compatible wrapper for UI_AppendMessage.
 *
 * Ownership: Takes ownership of `data` (a heap-allocated IdleMessageData*)
 * and frees it after use. Do not free it yourself.
 *
 * Parameters:
 *   data — Pointer to a heap-allocated IdleMessageData struct.
 *
 * Returns:
 *   G_SOURCE_REMOVE (FALSE) so the idle source fires only once.
 */
gboolean UI_AppendMessage_Idle(gpointer data);

/* ─── UI Signal Callbacks ────────────────────────────────────────────────── */

/*
 * UI_OnConnectClicked — Callback for the Connect button's "clicked" signal.
 * Calls Network_Connect() with the full application state.
 */
void UI_OnConnectClicked(GtkButton *button, gpointer userData);

/*
 * UI_OnSendClicked — Callback for the Send button's "clicked" signal.
 * Reads the entry text, calls Network_SendMessage(), and clears the entry.
 */
void UI_OnSendClicked(GtkButton *button, gpointer userData);

/*
 * UI_OnEntryKeyPressed — Callback for the Enter key in the message entry.
 * Calls UI_OnSendClicked() when GDK_KEY_Return is detected.
 */
gboolean UI_OnEntryKeyPressed(GtkEventControllerKey *controller,
                               guint keyval,
                               guint keycode,
                               GdkModifierType state,
                               gpointer userData);

/*
 * UI_OnWindowClose — Callback for the window's "close-request" signal.
 * Disconnects the network and joins the receive thread before the window closes.
 */
gboolean UI_OnWindowClose(GtkWindow *window, gpointer userData);

/* ─── Idle Message Helper ────────────────────────────────────────────────── */

/*
 * IdleMessageData — Payload passed through g_idle_add() from the receive thread.
 *
 * The receive thread heap-allocates one of these per incoming message,
 * g_idle_add() passes it to UI_AppendMessage_Idle(), which frees it.
 */
typedef struct {
    struct ChatApplicationState *appState;
    char                        *message;  /* Heap-allocated; freed by idle callback */
} IdleMessageData;

#endif /* UI_H */
