#include "clientMessages/BroadcastMessageMessage.h"

void BroadcastMessageMessage::ReadBody(int socket_fd) {
    ReadString(socket_fd, message);
}

void BroadcastMessageMessage::Process(Server *server, Client *client) {
    server->SendToAll(client->GetUserName(), GroupMessageMessage(client->GetUserName(), message));
}
