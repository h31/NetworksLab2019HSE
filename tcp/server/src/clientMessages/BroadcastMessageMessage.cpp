#include "clientMessages/BroadcastMessageMessage.h"
#include "Client.h"

bool BroadcastMessageMessage::ReadBody(SocketReader &reader) {
    return reader.ReadString(message);
}

bool BroadcastMessageMessage::Process(Server *server, Client *client) {
    server->SendToAll(client->GetUserName(), GroupMessageMessage(client->GetUserName(), message));
    return true;
}
