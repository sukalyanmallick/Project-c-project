/*
 * client/src/main.c — GTK4 Chat Client Entry Point
 *
 * Creates the GtkApplication, allocates the ChatApplicationState on
 * the heap (so it outlives activate()), builds the UI, and hands
 * control to the GTK main loop.
 *
 * Lifecycle:
 *   main()           — g_application_run()
 *   OnActivate()     — allocate state, build window + layout, show
 *   [GTK main loop]  — dispatches signal callbacks and idle sources
 *   UI_OnWindowClose — disconnect network, join recv thread
 *   [exit]           — g_free(appState)
 */

#include "../include/ui.h"

#include <gtk/gtk.h>
#include <stdlib.h>

/* ─── OnActivate ─────────────────────────────────────────────────────────── */

static void OnActivate(GtkApplication *app, gpointer userData) {
    (void)userData;

    /*
     * Heap-allocate so appState survives beyond this stack frame.
     * g_new0 zero-fills, which initialises isConnected to 0 and
     * all pointers to NULL without an explicit memset.
     */
    ChatApplicationState *appState = g_new0(ChatApplicationState, 1);

    /* Create and configure the top-level window */
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "AI Chatbot Client");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);

    /* Build the full widget hierarchy and connect all signal handlers */
    UI_BuildLayout(window, appState);

    gtk_window_present(GTK_WINDOW(window));
}

/* ─── main ───────────────────────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new(
        "com.example.chatbot",
        G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(OnActivate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}
