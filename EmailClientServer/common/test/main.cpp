#include <iostream>
#include "serialization/request.h"
#include "serialization/response.h"

using namespace std;
using namespace model;
using namespace request;
using namespace response;
using namespace serialization;

const string author = "aaa@aaa.com";
const string recipient = "bbb@aaa.com";
const string theme = "Wasup?";
const string body = "How are going?";
uint32_t id = 10;

template <class T, class U>
const T* cast(const U *from) {
    return reinterpret_cast<const T*>(from);
}

void checkSendEmailRequest() {
    Email email(theme, body, author, recipient);
    SendEmailRequest request(author, email);
    RequestSerializer serializer(&request);
    const util::encoded_message &message = serializer.serialize();
    RequestDeserializer deserializer(message.second.get(), INT_SIZE);
    const auto &pReceivedRequest = deserializer.parseRequest();
    const auto *receivedRequest = cast<SendEmailRequest>(pReceivedRequest.get());
    cout << "SendEmailRequest: " << (request == *receivedRequest ? "OK" : "FAIL") << endl;
}

void checkCheckEmailRequest() {
    CheckEmailRequest request(author);
    RequestSerializer serializer(&request);
    const util::encoded_message &message = serializer.serialize();
    RequestDeserializer deserializer(message.second.get(), INT_SIZE);
    const auto &pReceivedRequest = deserializer.parseRequest();
    const auto *receivedRequest = cast<CheckEmailRequest>(pReceivedRequest.get());
    cout << "CheckEmailRequest: " << (request == *receivedRequest ? "OK" : "FAIL") << endl;
}

void checkGetEmailRequest() {
    GetEmailRequest request(author, id);
    RequestSerializer serializer(&request);
    const util::encoded_message &message = serializer.serialize();
    RequestDeserializer deserializer(message.second.get(), INT_SIZE);
    const auto &pReceivedRequest = deserializer.parseRequest();
    const auto *receivedRequest = cast<GetEmailRequest>(pReceivedRequest.get());
    cout << "SendEmailRequest: " << (request == *receivedRequest ? "OK" : "FAIL") << endl;
}

void checkSendResponse() {
    auto body = make_shared<EmptyResponseBody>();
    Response response(body);
    ResponseSerializer serializer(&response);
    const util::encoded_message &message = serializer.serialize();
    ResponseDeserializer deserializer(message.second.get(), INT_SIZE);
    const shared_ptr<Response> &receivedResponse = deserializer.parseSendEmailResponse();
    cout << "SendEmailResponse: " << (response == *receivedResponse.get() ? "OK" : "FAIL") << endl;
}

void checkCheckResponse() {
    vector<EmailInfo> infos({{1, "a", "theme 1"}, {2, "b", "theme 2"}});
    auto body = make_shared<EmailInfosResponseBody>(infos);
    Response response(body);
    ResponseSerializer serializer(&response);
    const util::encoded_message &message = serializer.serialize();
    ResponseDeserializer deserializer(message.second.get(), INT_SIZE);
    const shared_ptr<Response> &receivedResponse = deserializer.parseCheckEmailResponse();
    cout << "CheckEmailResponse: " << (response == *receivedResponse.get() ? "OK" : "FAIL") << endl;
} 

void checkGetResponse() {
    Email email("theme", "body", "a", "b");
    auto body = make_shared<EmailResponseBody>(email);
    Response response(body);
    ResponseSerializer serializer(&response);
    const util::encoded_message &message = serializer.serialize();
    ResponseDeserializer deserializer(message.second.get(), INT_SIZE);
    const shared_ptr<Response> &receivedResponse = deserializer.parseGetEmailResponse();
    cout << "GetEmailResponse: " << (response == *receivedResponse.get() ? "OK" : "FAIL") << endl;
}

void checkErrorResponse() {
    auto body = make_shared<ErrorResponseBody>("it's an error");
    Response response(body);
    ResponseSerializer serializer(&response);
    const util::encoded_message &message = serializer.serialize();
    ResponseDeserializer deserializer(message.second.get(), INT_SIZE);
    const shared_ptr<Response> &receivedResponse = deserializer.parseSendEmailResponse();
    cout << "ErrorEmailResponse: " << (response == *receivedResponse.get() ? "OK" : "FAIL") << endl;
}

int main() {
    checkSendEmailRequest();
    checkCheckEmailRequest();
    checkGetEmailRequest();
    checkSendResponse();
    checkCheckResponse();
    checkGetResponse();
    checkErrorResponse();
}
