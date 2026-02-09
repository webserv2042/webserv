#include "../../includes/server/Client.hpp"

//CONSTRUCTOR(S)
Client::Client() {
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
        //stuff to copy
    }
    return (*this);
}

//DESTRUCTOR
Client::~Client() {
}

//PUBLIC

//PRIVATE
