//
// Created by Владислав Калинин on 11/02/2019.
//

#include "../include/Message.h"
#include "../include/constants.h"

std::string RegistryMessage::to_json_format() {
    return json({{TASK_ID, REGISTRY_TASK},
                 {NAME,     name},
                 {PASSWORD, password}}).dump();
}

std::string ResponseMessage::to_json_format() {
    if (cause.empty()) {
        return json({{STATUS, status}}).dump();
    }
    return json({{STATUS, status},
                 {CAUSE,  cause}}).dump();
}

std::string AuthenticationMessage::to_json_format() {
    return json({{TASK_ID, AUTHORIZATION_TASK},
                 {NAME,     name},
                 {PASSWORD, password}}).dump();
}

std::string TextMessage::to_json_format() {
    return json({{TASK_ID, BROADCAST_TASK},
                 {SENDER,  sender},
                 {MESSAGE, message}}).dump();
}

std::string PrivateMessage::to_json_format() {
    return json({{TASK_ID, PRIVATE_TASK},
                 {SENDER,   sender},
                 {RECEIVER, reciever},
                 {MESSAGE,  message}}).dump();
}

std::string KickMessage::to_json_format() {
    return json({{TASK_ID, KICK_TASK},
                 {NAME, name}}).dump();
}
