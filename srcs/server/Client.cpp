#include "../../includes/server/Client.hpp"

//CONSTRUCTOR(S)
Client::Client() {
    clientFd = -1;
    bytesSent = 0;
    buffSize = 0;
}

Client::Client(int new_client_fd) : clientFd(new_client_fd){
    bytesSent = 0;
    buffSize = 0;
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
        bytesSent = to_copy.bytesSent;
        buffSize = to_copy.buffSize;
    }
    return (*this);
}

//DESTRUCTOR
Client::~Client() {
}

//PUBLIC

/// @brief remet les donnees du client a zero pour qu'il puisse attendre de nouvelle connections\n
/// cela ne supprime pas le client ni son fd, juste ses donnees
void    Client::resetClient()
{
    buffSize = 0;
    bytesSent = 0;
    readBuff.clear();
    writeBuff.clear();
    clientState = DONE;
}

//PRIVATE
