#include "serverMessages/ErrorMessage.h"
#include "serverMessages/LoginSuccessMessage.h"
#include "clientMessages/LoginMessage.h"
#include "Client.h"

bool LoginMessage::ReadBody(SocketReader &reader) {
    return reader.ReadString(user_name);
}

bool LoginMessage::Process(Server *server, Client *client) {
    if (client->IsLoggedIn()) {
        return client->Send(ErrorMessage("You are already logged in"));
    }
    if (!server->AddClient(user_name, client)) {
        return client->Send(ErrorMessage("This user name is already busy"));
    }
    client->SetUserName(user_name);
    return client->Send(LoginSuccessMessage());
}
