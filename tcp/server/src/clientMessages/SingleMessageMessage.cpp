#include "serverMessages/ErrorMessage.h"
#include "clientMessages/SingleMessageMessage.h"

void SingleMessageMessage::ReadBody(int socket_fd) {
    ReadString(socket_fd, user_name);
    ReadString(socket_fd, message);
}

void SingleMessageMessage::Process(Server *server, Client *client) {
    if (!server->SendTo(client->GetUserName(), user_name, MessageMessage(user_name, message))) {
        ErrorMessage("The recipient's user name does not exist").Write(client->GetSocket());
    }
}
