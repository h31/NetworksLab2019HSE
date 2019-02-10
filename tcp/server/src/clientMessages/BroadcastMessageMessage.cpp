#include "clientMessages/BroadcastMessageMessage.h"
#include "Client.h"

bool BroadcastMessageMessage::ReadBody(int socket_fd) {
    return ReadString(socket_fd, message);
}

bool BroadcastMessageMessage::Process(Server *server, Client *client) {
    server->SendToAll(client->GetUserName(), GroupMessageMessage(client->GetUserName(), message));
    return true;
}
