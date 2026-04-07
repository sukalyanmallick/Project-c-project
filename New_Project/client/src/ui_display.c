/*
 * client/src/ui_display.c — Chat History Display Functions
 *
 * Implements UI_AppendMessage() and the g_idle_add()-compatible wrapper
 * UI_AppendMessage_Idle(). These are the only two functions that write
 * to the GtkTextView; all other UI files call through them.
 *
 * Threading rule:
 *   UI_AppendMessage      — main thread only.
 *   UI_AppendMessage_Idle — called by the GTK main loop via g_idle_add();
 *                           therefore also main-thread-safe.
 */

#include "../include/ui.h"

#include <gtk/gtk.h>

/* ─── UI_AppendMessage ───────────────────────────────────────────────────── */

void UI_AppendMessage(ChatApplicationState *appState, const char *message) {
    g_assert(appState != NULL);
    g_assert(message  != NULL);

    GtkTextBuffer *buf = gtk_text_view_get_buffer(
                             GTK_TEXT_VIEW(appState->textView));

    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buf, &end);

    gtk_text_buffer_insert(buf, &end, message, -1);
    gtk_text_buffer_insert(buf, &end, "\n",    -1);

    /*
     * Auto-scroll to the bottom so the newest message is always visible.
     * Re-fetch the end iterator because it advanced after the inserts.
     */
    gtk_text_buffer_get_end_iter(buf, &end);
    GtkTextMark *mark = gtk_text_buffer_create_mark(buf, NULL, &end, FALSE);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(appState->textView),
                                 mark, 0.0, TRUE, 0.0, 1.0);
    gtk_text_buffer_delete_mark(buf, mark);
}

/* ─── UI_AppendMessage_Idle ──────────────────────────────────────────────── */

gboolean UI_AppendMessage_Idle(gpointer data) {
    IdleMessageData *payload = (IdleMessageData *)data;

    UI_AppendMessage(payload->appState, payload->message);

    g_free(payload->message);
    g_free(payload);

    return G_SOURCE_REMOVE; /* Fire once, then remove from the main loop. */
}
