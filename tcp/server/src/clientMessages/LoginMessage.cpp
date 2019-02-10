#include "serverMessages/ErrorMessage.h"
#include "serverMessages/LoginSuccessMessage.h"
#include "clientMessages/LoginMessage.h"

void LoginMessage::ReadBody(int socket_fd) {
    ReadString(socket_fd, user_name);
}

void LoginMessage::Process(Server *server, Client *client) {
    if (client->IsLoggedIn()) {
        ErrorMessage("You are already logged in").Write(client->GetSocket());
    } else if (!server->AddClient(user_name, client)) {
        ErrorMessage("This user name is already busy").Write(client->GetSocket());
    } else {
        client->SetUserName(user_name);
        LoginSuccessMessage().Write(client->GetSocket());
    }
}
