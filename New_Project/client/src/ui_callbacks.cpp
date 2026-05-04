/*
 * client/src/ui_callbacks.cpp — GTK4 Signal Callbacks (C++ / SOLID)
 *
 * SOLID Principles Applied:
 *
 *   Single Responsibility (SRP):
 *     Each callback has exactly one job — handle one user interaction.
 *     They delegate network operations via the ISP interfaces
 *     (IConnectionManager, IMessageSender) stored in ChatApplicationState.
 *
 *   Dependency Inversion (DIP):
 *     Callbacks access appState->connector (IConnectionManager*) and
 *     appState->sender (IMessageSender*). They never call TcpNetworkClient
 *     methods directly, so any conforming implementation can be swapped in.
 *
 *   Interface Segregation (ISP):
 *     UI_OnConnectClicked only uses IConnectionManager.
 *     UI_OnSendClicked    only uses IMessageSender + IConnectionManager::isConnected.
 *     Neither callback depends on the full TcpNetworkClient class.
 *
 * Note:
 *   GTK4 signal callbacks must match C-linkage-compatible signatures.
 *   They live in namespace ChatBot but are registered via G_CALLBACK().
 */

#include "../include/ui.hpp"

#include <gtk/gtk.h>
#include <cstring>

namespace ChatBot {

/* ─── UI_OnConnectClicked ────────────────────────────────────────────────── */

void UI_OnConnectClicked(GtkButton* button, gpointer userData) {
    (void)button; /* Required by GTK signal signature; not used. */

    auto* appState = static_cast<ChatApplicationState*>(userData);

    /* DIP: call through IConnectionManager interface. */
    appState->connector->connect();
}

/* ─── UI_OnSendClicked ───────────────────────────────────────────────────── */

void UI_OnSendClicked(GtkButton* button, gpointer userData) {
    (void)button;

    auto* appState = static_cast<ChatApplicationState*>(userData);

    /* Guard: refuse to send when disconnected (ISP: only isConnected needed). */
    if (!appState->connector->isConnected()) {
        UI_AppendMessage(appState, "Not connected. Click Connect first.");
        return;
    }

    const char* inputText = gtk_editable_get_text(
                                GTK_EDITABLE(appState->messageEntry));

    if (std::strlen(inputText) == 0) {
        return;
    }

    /* DIP: call through IMessageSender interface. */
    if (!appState->sender->sendMessage(inputText)) {
        UI_AppendMessage(appState, "Failed to send message.");
        return;
    }

    /* Echo the sent message in the local chat view. */
    gchar* echo = g_strdup_printf("You: %s", inputText);
    UI_AppendMessage(appState, echo);
    g_free(echo);

    /*
     * "bye" — disconnect after the message is sent so the server
     * receives the farewell before the socket is closed.
     */
    if (std::strcmp(inputText, "bye") == 0) {
        appState->connector->disconnect();
        UI_AppendMessage(appState, "Disconnected.");
    }

    gtk_editable_set_text(GTK_EDITABLE(appState->messageEntry), "");
}

/* ─── UI_OnEntryKeyPressed ───────────────────────────────────────────────── */

gboolean UI_OnEntryKeyPressed(GtkEventControllerKey* controller,
                               guint keyval,
                               guint keycode,
                               GdkModifierType state,
                               gpointer userData) {
    (void)controller;
    (void)keycode;
    (void)state;

    if (keyval == GDK_KEY_Return) {
        UI_OnSendClicked(nullptr, userData);
        return TRUE; /* Event handled; stop further propagation. */
    }

    return FALSE;
}

/* ─── UI_OnWindowClose ───────────────────────────────────────────────────── */

gboolean UI_OnWindowClose(GtkWindow* window, gpointer userData) {
    (void)window;

    auto* appState = static_cast<ChatApplicationState*>(userData);

    /*
     * Disconnect first — atomically clears connectedFlag_ so the receive
     * thread exits its recv() loop on the next iteration.
     */
    if (appState->connector->isConnected()) {
        appState->connector->disconnect();
    }

    return FALSE; /* Allow the window-close to proceed. */
}

} // namespace ChatBot
