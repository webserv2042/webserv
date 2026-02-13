#include "../../includes/server/Client.hpp"

//CONSTRUCTOR(S)
Client::Client() {
    clientFd = -1;
    bytesSent = 0;
}

Client::Client(int new_client_fd) : clientFd(new_client_fd){
}

//COPY CONSTRUCTOR
Client::Client(const Client& to_copy) {
    *this = to_copy;
}

//COPY ASSIGNMENT
Client& Client::operator=(const Client& to_copy) {
    if (this != &to_copy)
    {
        clientFd = to_copy.clientFd;
        clientState = to_copy.clientState;
        readBuff = to_copy.readBuff;
        writeBuff = to_copy.writeBuff;
    }
    return (*this);
}

//DESTRUCTOR
Client::~Client() {
}

//PUBLIC

//PRIVATE
