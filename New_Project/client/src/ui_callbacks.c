/*
 * client/src/ui_callbacks.c — GTK4 Signal Callback Implementations
 *
 * Implements the four signal callbacks wired up in ui_layout.c:
 *
 *   UI_OnConnectClicked    — Connect button "clicked"
 *   UI_OnSendClicked       — Send button "clicked"
 *   UI_OnEntryKeyPressed   — Entry "key-pressed" (Enter → Send)
 *   UI_OnWindowClose       — Window "close-request"
 *
 * Each callback receives the full ChatApplicationState via the
 * userData / gpointer parameter and delegates to the appropriate
 * network or display helper. No GTK widget is stored in a global.
 */

#include "../include/ui.h"
#include "../include/network_client.h"

#include <gtk/gtk.h>
#include <string.h>

/* ─── UI_OnConnectClicked ────────────────────────────────────────────────── */

void UI_OnConnectClicked(GtkButton *button, gpointer userData) {
    (void)button; /* Required by GTK signal signature; not used. */

    ChatApplicationState *appState = (ChatApplicationState *)userData;
    Network_Connect(appState);
}

/* ─── UI_OnSendClicked ───────────────────────────────────────────────────── */

void UI_OnSendClicked(GtkButton *button, gpointer userData) {
    (void)button;

    ChatApplicationState *appState = (ChatApplicationState *)userData;

    /* Guard: refuse to send when disconnected. */
    if (!g_atomic_int_get(&appState->network.isConnected)) {
        UI_AppendMessage(appState, "Not connected. Click Connect first.");
        return;
    }

    const char *inputText = gtk_editable_get_text(
                                GTK_EDITABLE(appState->messageEntry));

    if (strlen(inputText) == 0) {
        return;
    }

    if (!Network_SendMessage(&appState->network, inputText)) {
        UI_AppendMessage(appState, "Failed to send message.");
        return;
    }

    /* Echo the sent message in the local chat view. */
    char *echo = g_strdup_printf("You: %s", inputText);
    UI_AppendMessage(appState, echo);
    g_free(echo);

    /*
     * "bye" — disconnect after the message is sent so the server
     * receives it before the socket is closed.
     */
    if (strcmp(inputText, "bye") == 0) {
        Network_Disconnect(&appState->network);
        UI_AppendMessage(appState, "Disconnected.");
    }

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

    if (keyval == GDK_KEY_Return) {
        UI_OnSendClicked(NULL, userData);
        return TRUE;  /* Event handled; stop further propagation. */
    }

    return FALSE;
}

/* ─── UI_OnWindowClose ───────────────────────────────────────────────────── */

gboolean UI_OnWindowClose(GtkWindow *window, gpointer userData) {
    (void)window;

    ChatApplicationState *appState = (ChatApplicationState *)userData;

    /*
     * Disconnect first — this atomically clears isConnected so the
     * receive thread exits its recv() loop on the next iteration.
     */
    if (g_atomic_int_get(&appState->network.isConnected)) {
        Network_Disconnect(&appState->network);
    }

    /*
     * Join the receive thread before the window is destroyed to prevent
     * a use-after-free if a pending g_idle_add() callback fires late.
     */
    if (appState->network.receiveThread != NULL) {
        g_thread_join(appState->network.receiveThread);
        appState->network.receiveThread = NULL;
    }

    return FALSE; /* Allow the window-close to proceed. */
}
