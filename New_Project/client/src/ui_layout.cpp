/*
 * client/src/ui_layout.cpp — GTK4 Widget Construction (C++ / SOLID)
 *
 * SOLID Principles Applied:
 *
 *   Single Responsibility (SRP):
 *     UI_BuildLayout has exactly one job: construct the widget hierarchy
 *     and wire all signal handlers. It does not contain network logic
 *     or business decisions.
 *
 *   Dependency Inversion (DIP):
 *     Layout construction stores widget pointers in ChatApplicationState
 *     and connects callbacks that access IConnectionManager* / IMessageSender*.
 *     The layout function itself never calls Winsock APIs.
 *
 * Widget tree:
 *
 *   GtkApplicationWindow
 *   └── GtkBox (vertical, vbox)
 *       ├── GtkScrolledWindow
 *       │   └── GtkTextView        — read-only chat history
 *       ├── GtkEntry               — message input field
 *       └── GtkBox (horizontal)
 *           ├── GtkButton "Connect"
 *           └── GtkButton "Send"
 *
 * All widget pointers that need to be accessed later (textView,
 * messageEntry) are stored in appState. No file-scope statics are used.
 */

#include "../include/ui.hpp"
#include "../../shared/constants.hpp"

#include <gtk/gtk.h>

namespace ChatBot {

/* ─── UI_BuildLayout ─────────────────────────────────────────────────────── */

void UI_BuildLayout(GtkWidget* window, ChatApplicationState* appState) {

    /* ── Outer vertical container ── */
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start (vbox, 10);
    gtk_widget_set_margin_end   (vbox, 10);
    gtk_widget_set_margin_top   (vbox, 10);
    gtk_widget_set_margin_bottom(vbox, 10);
    gtk_window_set_child(GTK_WINDOW(window), vbox);

    /* ── Scrolled window + read-only chat history ── */
    GtkWidget* scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled, TRUE);

    appState->textView = gtk_text_view_new();
    gtk_text_view_set_editable    (GTK_TEXT_VIEW(appState->textView), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(appState->textView), FALSE);
    gtk_text_view_set_wrap_mode   (GTK_TEXT_VIEW(appState->textView),
                                   GTK_WRAP_WORD);
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW(scrolled),
                                   appState->textView);
    gtk_box_append(GTK_BOX(vbox), scrolled);

    /* ── Message entry with Enter-key support ── */
    appState->messageEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(appState->messageEntry),
                                   "Type a message...");
    gtk_box_append(GTK_BOX(vbox), appState->messageEntry);

    GtkEventController* keyCtrl = gtk_event_controller_key_new();
    g_signal_connect(keyCtrl, "key-pressed",
                     G_CALLBACK(UI_OnEntryKeyPressed), appState);
    gtk_widget_add_controller(appState->messageEntry, keyCtrl);

    /* ── Button row ── */
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(hbox), TRUE);
    gtk_box_append(GTK_BOX(vbox), hbox);

    GtkWidget* connectBtn = gtk_button_new_with_label("Connect");
    g_signal_connect(connectBtn, "clicked",
                     G_CALLBACK(UI_OnConnectClicked), appState);
    gtk_box_append(GTK_BOX(hbox), connectBtn);

    GtkWidget* sendBtn = gtk_button_new_with_label("Send");
    g_signal_connect(sendBtn, "clicked",
                     G_CALLBACK(UI_OnSendClicked), appState);
    gtk_box_append(GTK_BOX(hbox), sendBtn);

    /* ── Window close handler ── */
    g_signal_connect(window, "close-request",
                     G_CALLBACK(UI_OnWindowClose), appState);

    /* ── Initial welcome messages ── */
    gchar* welcomeMsg = g_strdup_printf(
        "Welcome! Click Connect to join the chatbot server (%s:%d).",
        DEFAULT_SERVER_IP, DEFAULT_SERVER_PORT);
    UI_AppendMessage(appState, welcomeMsg);
    g_free(welcomeMsg);

    UI_AppendMessage(appState,
        "Type your message and press Enter or click Send.");
}

} // namespace ChatBot
