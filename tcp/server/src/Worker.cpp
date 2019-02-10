#include "clientMessages/ClientMessages.h"
#include "Worker.h"

void Worker::operator()(Server *server, Client *client) {
    while (true) {
        ClientMessage *message = nullptr;
        if (!client->Receive(&message)) {
            break;
        }
        if (!message) {
            continue;
        }
        if (!message->Process(server, client)) {
            break;
        }
    }
    server->RemoveClient(client->GetUserName());
}
