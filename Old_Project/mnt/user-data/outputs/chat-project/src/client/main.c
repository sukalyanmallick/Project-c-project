/*
 * main.c — GTK4 Chat Client Entry Point
 *
 * Initialises the GtkApplication, creates the main window, calls
 * UI_BuildLayout() to construct the widget tree, and then hands
 * control to the GTK main loop.
 *
 * The ChatApplicationState is stack-allocated here and lives for the
 * full duration of the application. No global variables are used.
 */

#include "ui.h"
#include "network_client.h"

/* ─── app_activate ───────────────────────────────────────────────────────── */

/*
 * app_activate — Called by GtkApplication when the application starts.
 *
 * Creates the main window, zero-initialises the application state,
 * builds the widget layout, and presents the window.
 *
 * Parameters:
 *   gapp     — The GtkApplication instance.
 *   userData — Unused; required by the GApplication activate signal.
 */
static void app_activate(GApplication *gapp, gpointer userData) {
    (void)userData;

    /* Zero-initialise: sets socket to 0, isConnected to 0, pointers to NULL */
    ChatApplicationState appState = {0};

    /* Create and configure the main window */
    GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(gapp));
    gtk_window_set_title(GTK_WINDOW(window), "GTK4 AI Chat Client");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);

    /*
     * Build all widgets and attach them to the window.
     * Signal callbacks are wired up inside UI_BuildLayout().
     */
    UI_BuildLayout(window, &appState);

    gtk_window_present(GTK_WINDOW(window));
}

/* ─── main ───────────────────────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    GtkApplication *gtkApp = gtk_application_new(
                                 "com.example.gtkchat",
                                 G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(gtkApp, "activate", G_CALLBACK(app_activate), NULL);

    int exitStatus = g_application_run(G_APPLICATION(gtkApp), argc, argv);

    g_object_unref(gtkApp);
    return exitStatus;
}
