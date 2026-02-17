#include "../../includes/server/Client.hpp"
#include "../../includes/Webserv.hpp"

//CONSTRUCTOR(S)
Client::Client() {
    clientFd = -1;
    bytesSent = 0;
    buffSize = 0;
    lastActivity = time(NULL);
}

Client::Client(int new_client_fd) : clientFd(new_client_fd){
    bytesSent = 0;
    buffSize = 0;
    lastActivity = time(NULL);
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
        lastActivity = to_copy.lastActivity;
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
    // lastActivity = time(NULL);
    readBuff.clear();
    writeBuff.clear();
    clientState = DONE;
}

/// @brief verifie si le timeout d'un client a ete depasse
/// @return TIMEOUT(1) si le timout est depasse, ou NO_TIMOUT s'il ne l'est pas
int    Client::idleTimeout()
{
   time_t current_time = time(NULL);

    // std::cout << "current time: " << current_time <<  " last activity: " << lastActivity << std::endl;
    double seconds = difftime(current_time, lastActivity);
    // std::cout << "client " << clientFd << ", time since last activity: " << seconds << std::endl;

    if (seconds >= IDLE_TIMEOUT)
        return TIMEOUT;

    return NO_TIMEOUT;
}

//PRIVATE
