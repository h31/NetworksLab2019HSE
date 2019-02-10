#include "serverMessages/ErrorMessage.h"
#include "clientMessages/SingleMessageMessage.h"
#include "Client.h"

bool SingleMessageMessage::ReadBody(int socket_fd) {
    return ReadString(socket_fd, user_name) && ReadString(socket_fd, message);
}

bool SingleMessageMessage::Process(Server *server, Client *client) {
    if (!server->SendTo(client->GetUserName(), user_name, MessageMessage(user_name, message))) {
        return client->Send(ErrorMessage("The recipient's user name does not exist"));
    }
    return true;
}
