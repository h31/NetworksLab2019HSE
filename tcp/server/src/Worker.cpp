#include "clientMessages/ClientMessages.h"
#include "Worker.h"

void Worker::operator()(Server *server, Client *client) {
    while (true) {
        ClientMessageType type;
        ssize_t readSize = read(client->GetSocket(), &type, sizeof(type));
        if (readSize < sizeof(type)) {
            perror("Error on reading");
            return;
        }
        ClientMessage *message = nullptr;
        switch (type) {
            case ClientMessageType::LOGIN:
                message = new LoginMessage();
                break;
            case ClientMessageType::SINGLE_MESSAGE:
                message = new SingleMessageMessage();
                break;
            case ClientMessageType::BROADCAST_MESSAGE:
                message = new BroadcastMessageMessage();
                break;
            default:
                std::cout << "Incorrect message type: " << (unsigned char) type << std::endl;
                continue;
        }
        message->ReadBody(client->GetSocket());
        message->Process(server, client);
    }
}
