/*
 * ui.c — GTK4 Chat Client UI Layer Implementation
 *
 * Builds the GTK4 widget hierarchy and implements all signal callbacks.
 * This layer is intentionally decoupled from Winsock: it calls into
 * network_client.c for all network operations and never touches
 * socket APIs directly.
 *
 * Widget hierarchy:
 *
 *   GtkApplicationWindow
 *   └── GtkBox (vertical, vbox)
 *       ├── GtkScrolledWindow
 *       │   └── GtkTextView  (read-only chat history)
 *       ├── GtkEntry         (message input)
 *       └── GtkBox (horizontal, hbox)
 *           ├── GtkButton    "Connect"
 *           └── GtkButton    "Send"
 */

#include "ui.h"
#include "network_client.h"
#include "../shared/constants.h"

#include <string.h>

/* ─── UI_AppendMessage ───────────────────────────────────────────────────── */

void UI_AppendMessage(ChatApplicationState *appState, const char *message) {
    g_assert(appState != NULL);
    g_assert(message  != NULL);

    GtkTextBuffer *textBuffer = gtk_text_view_get_buffer(
                                    GTK_TEXT_VIEW(appState->textView));

    GtkTextIter endIterator;
    gtk_text_buffer_get_end_iter(textBuffer, &endIterator);

    /* Append the message text followed by a newline */
    gtk_text_buffer_insert(textBuffer, &endIterator, message, -1);
    gtk_text_buffer_insert(textBuffer, &endIterator, "\n",    -1);

    /*
     * Scroll the view to the bottom after inserting so the newest message
     * is always visible. Re-fetch end iter because it moved after insert.
     */
    gtk_text_buffer_get_end_iter(textBuffer, &endIterator);
    GtkTextMark *endMark = gtk_text_buffer_create_mark(
                               textBuffer, NULL, &endIterator, FALSE);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(appState->textView),
                                 endMark, 0.0, TRUE, 0.0, 1.0);
    gtk_text_buffer_delete_mark(textBuffer, endMark);
}

/* ─── UI_AppendMessage_Idle ──────────────────────────────────────────────── */

gboolean UI_AppendMessage_Idle(gpointer data) {
    /*
     * This function is called by the GTK main loop via g_idle_add().
     * It runs on the main thread, so it is safe to call GTK functions here.
     * We take ownership of `data` and free it before returning.
     */
    IdleMessageData *payload = (IdleMessageData *)data;

    UI_AppendMessage(payload->appState, payload->message);

    g_free(payload->message);
    g_free(payload);

    /*
     * Return G_SOURCE_REMOVE so this idle source fires exactly once
     * and is automatically removed from the main loop.
     */
    return G_SOURCE_REMOVE;
}

/* ─── UI_OnConnectClicked ────────────────────────────────────────────────── */

void UI_OnConnectClicked(GtkButton *button, gpointer userData) {
    (void)button; /* Signal parameter required by GTK; not used here. */

    ChatApplicationState *appState = (ChatApplicationState *)userData;
    Network_Connect(appState);
}

/* ─── UI_OnSendClicked ───────────────────────────────────────────────────── */

void UI_OnSendClicked(GtkButton *button, gpointer userData) {
    (void)button;

    ChatApplicationState *appState = (ChatApplicationState *)userData;

    /*
     * Guard: refuse to send if not connected.
     * Use atomic read because the receive thread may update isConnected.
     */
    if (!g_atomic_int_get(&appState->network.isConnected)) {
        UI_AppendMessage(appState, "Not connected. Click Connect first.");
        return;
    }

    const char *inputText = gtk_editable_get_text(
                                GTK_EDITABLE(appState->messageEntry));

    /* Ignore empty submissions */
    if (strlen(inputText) == 0) {
        return;
    }

    /* Send over the network */
    if (!Network_SendMessage(&appState->network, inputText)) {
        UI_AppendMessage(appState, "Failed to send message.");
        return;
    }

    /* Echo the sent message in the chat view so the user sees it */
    char *echoLine = g_strdup_printf("You: %s", inputText);
    UI_AppendMessage(appState, echoLine);
    g_free(echoLine);

    /*
     * Handle "bye" locally: disconnect after the message is sent so the
     * server receives it before the socket closes.
     */
    if (strcmp(inputText, "bye") == 0) {
        Network_Disconnect(&appState->network);
        UI_AppendMessage(appState, "Disconnected.");
    }

    /* Clear the entry field ready for the next message */
    gtk_editable_set_text(GTK_EDITABLE(appState->messageEntry), "");
}

/* ─── UI_OnEntryKeyPressed ───────────────────────────────────────────────── */

gboolean UI_OnEntryKeyPressed(GtkEventControllerKey *controller,
                               guint keyval,
                               guint keycode,
                               GdkModifierType state,
                               gpointer userData) {
    (void)controller;
    (void)keycode;
    (void)state;

    /*
     * Treat Enter as a Send action so the user doesn't have to click the button.
     * Return TRUE to mark the event as handled and prevent further propagation.
     */
    if (keyval == GDK_KEY_Return) {
        UI_OnSendClicked(NULL, userData);
        return TRUE;
    }

    return FALSE;
}

/* ─── UI_OnWindowClose ───────────────────────────────────────────────────── */

gboolean UI_OnWindowClose(GtkWindow *window, gpointer userData) {
    (void)window;

    ChatApplicationState *appState = (ChatApplicationState *)userData;

    /*
     * Disconnect the network first (sets isConnected = 0 atomically),
     * which signals the receive thread to exit its loop naturally.
     */
    if (g_atomic_int_get(&appState->network.isConnected)) {
        Network_Disconnect(&appState->network);
    }

    /*
     * Join the receive thread to ensure it has exited before the window
     * and its widgets are destroyed. This prevents use-after-free in
     * UI_AppendMessage_Idle() if a pending idle callback fires late.
     */
    if (appState->network.receiveThread != NULL) {
        g_thread_join(appState->network.receiveThread);
        appState->network.receiveThread = NULL;
    }

    return FALSE; /* Allow the window close to proceed */
}

/* ─── UI_BuildLayout ─────────────────────────────────────────────────────── */

void UI_BuildLayout(GtkWidget *window, ChatApplicationState *appState) {
    /* ── Outer vertical container ── */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(vbox,   10);
    gtk_widget_set_margin_end(vbox,     10);
    gtk_widget_set_margin_top(vbox,     10);
    gtk_widget_set_margin_bottom(vbox,  10);
    gtk_window_set_child(GTK_WINDOW(window), vbox);

    /* ── Scrolled window containing the read-only chat history ── */
    GtkWidget *scrolledWindow = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolledWindow, TRUE);

    appState->textView = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(appState->textView), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(appState->textView), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(appState->textView), GTK_WRAP_WORD);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow),
                                  appState->textView);
    gtk_box_append(GTK_BOX(vbox), scrolledWindow);

    /* ── Message entry field ── */
    appState->messageEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(appState->messageEntry),
                                   "Type a message...");
    gtk_box_append(GTK_BOX(vbox), appState->messageEntry);

    /* Enter key controller attached to the entry */
    GtkEventController *keyController = gtk_event_controller_key_new();
    g_signal_connect(keyController, "key-pressed",
                     G_CALLBACK(UI_OnEntryKeyPressed), appState);
    gtk_widget_add_controller(appState->messageEntry, keyController);

    /* ── Horizontal button row ── */
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(hbox), TRUE);
    gtk_box_append(GTK_BOX(vbox), hbox);

    GtkWidget *connectButton = gtk_button_new_with_label("Connect");
    g_signal_connect(connectButton, "clicked",
                     G_CALLBACK(UI_OnConnectClicked), appState);
    gtk_box_append(GTK_BOX(hbox), connectButton);

    GtkWidget *sendButton = gtk_button_new_with_label("Send");
    g_signal_connect(sendButton, "clicked",
                     G_CALLBACK(UI_OnSendClicked), appState);
    gtk_box_append(GTK_BOX(hbox), sendButton);

    /* ── Window close signal ── */
    g_signal_connect(window, "close-request",
                     G_CALLBACK(UI_OnWindowClose), appState);

    /* ── Welcome message ── */
    UI_AppendMessage(appState,
        "Welcome! Click Connect to join the chatbot server ("
        DEFAULT_SERVER_IP ":" G_STRINGIFY(DEFAULT_SERVER_PORT) ").");
    UI_AppendMessage(appState,
        "Type your message and press Enter or click Send.");
}
