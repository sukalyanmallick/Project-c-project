/*
 * client/src/main.cpp — GTK4 Chat Client Entry Point (C++ / SOLID)
 *
 * SOLID Principles Applied:
 *
 *   Dependency Inversion (DIP):
 *     This is the Composition Root. main() creates the concrete
 *     TcpNetworkClient and stores its addresses as IConnectionManager*
 *     and IMessageSender* inside ChatApplicationState. All downstream
 *     code (UI callbacks, layout) depends only on those interfaces,
 *     never on TcpNetworkClient directly.
 *
 *   Single Responsibility (SRP):
 *     OnActivate() has one job: allocate state, wire the interfaces,
 *     build the window/layout, and show it. Nothing else.
 *
 *   Open/Closed (OCP):
 *     Swapping the network backend (e.g. to a mock or a TLS client)
 *     requires changing only the make_unique call here — no other
 *     file needs to change.
 *
 * Lifecycle:
 *   main()           → g_application_run()
 *   OnActivate()     → allocate state + network + window + layout
 *   [GTK main loop]  → dispatches signal callbacks and idle sources
 *   UI_OnWindowClose → disconnect network (g_thread_join is implicit
 *                      since receiveThread exits its loop when flag clears)
 *   [exit]           → delete network client, g_free appState
 */

#include "../include/ui.hpp"
#include "../include/network_client.hpp"

#include <gtk/gtk.h>
#include <memory>

namespace ChatBot {

/* ─── OnActivate ─────────────────────────────────────────────────────────── */

static void OnActivate(GtkApplication* app, gpointer /*userData*/) {

    /*
     * Heap-allocate so appState survives beyond this stack frame.
     * g_new0 zero-fills (initialises all pointers to nullptr).
     */
    auto* appState = g_new0(ChatApplicationState, 1);

    /*
     * Composition Root — create the concrete network client and inject it
     * as both ISP interfaces (DIP).
     *
     * Use a raw heap allocation managed alongside appState so the lifetime
     * matches: the TcpNetworkClient is deleted when the window closes.
     * (Storing it in a unique_ptr inside appState would require a C++ struct
     *  but GTK callbacks use gpointer/g_new0, so we manage it manually.)
     */
    auto* networkClient = new TcpNetworkClient(appState);
    appState->connector = networkClient;   /* IConnectionManager* */
    appState->sender    = networkClient;   /* IMessageSender*     */

    /* Create and configure the top-level window. */
    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "AI Chatbot Client");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);

    /*
     * Store the network client pointer on the window so it can be
     * deleted safely in a destroy callback after UI_OnWindowClose.
     */
    g_object_set_data_full(G_OBJECT(window), "network-client",
                           networkClient,
                           [](gpointer p) {
                               delete static_cast<TcpNetworkClient*>(p);
                           });

    /* g_free appState when the window is destroyed. */
    g_object_set_data_full(G_OBJECT(window), "app-state",
                           appState,
                           [](gpointer p) {
                               g_free(p);
                           });

    /* Build the full widget hierarchy and connect all signal handlers. */
    UI_BuildLayout(window, appState);

    gtk_window_present(GTK_WINDOW(window));
}

} // namespace ChatBot

/* ─── main ───────────────────────────────────────────────────────────────── */

int main(int argc, char* argv[]) {
    GtkApplication* app = gtk_application_new(
        "com.example.chatbot",
        G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate",
                     G_CALLBACK(ChatBot::OnActivate), nullptr);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}
