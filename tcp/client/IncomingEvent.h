#include <utility>

#ifndef CLIENT_INCOMINGEVENT_H
#define CLIENT_INCOMINGEVENT_H

enum class IncomingEventType {
    ERROR = 0,
    NEW_MESSAGE = 1,
    MESSAGE_CONFIRMED = 2,
    SERVER_DISCONNECTED = 3
};

class IncomingEvent {
public:
    IncomingEvent() : anything_happened(false) {}
    IncomingEvent(IncomingEventType event_type, std::string message): event_type(event_type), message(std::move(message)) {}

    IncomingEventType get_event() { return event_type; }
    std::string get_message() { return message; }
    bool is_anything_happened() {
        return anything_happened;
    }

private:
    bool anything_happened = false;
    std::string message ;
    IncomingEventType event_type = IncomingEventType::ERROR;
};

#endif //CLIENT_INCOMINGEVENT_H
