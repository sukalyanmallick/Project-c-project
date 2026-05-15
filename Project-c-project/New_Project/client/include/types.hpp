/*
 * client/include/types.hpp — Shared Client Data Types
 *
 * This file contains structures used by both the network and UI layers
 * but DOES NOT depend on any external libraries like GTK or GLib.
 */

#ifndef TYPES_HPP
#define TYPES_HPP

namespace ChatBot {

class IConnectionManager;
class IMessageSender;

/*
 * ChatApplicationState — Central state for the client.
 * Using forward declarations for widgets to avoid GTK dependency here.
 */
struct ChatApplicationState {
    void*               textView;       /* GtkWidget* (opaque here) */
    void*               messageEntry;   /* GtkWidget* (opaque here) */
    IConnectionManager* connector;
    IMessageSender*     sender;
};

/*
 * IdleMessageData — Payload for cross-thread messaging.
 */
struct IdleMessageData {
    ChatApplicationState* appState;
    char*                 message;
};

} // namespace ChatBot

#endif /* TYPES_HPP */
