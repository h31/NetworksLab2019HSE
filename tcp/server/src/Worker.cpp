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
        bool result = message->Process(server, client);
        delete message;
        if (!result) {
            break;
        }
    }
    server->RemoveClient(client->GetUserName());
    delete client;
}
