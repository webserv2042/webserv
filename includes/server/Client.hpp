#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Request.hpp"
#include "Config.hpp"
#include <vector>
#include <ctime>

#define IDLE_TIMEOUT 60 //timeout d'un client sans aucune activité
#define NO_TIMEOUT 0
#define TIMEOUT 1

typedef enum e_client_state
{
    CONNEXION_ACCEPTED,
    READING_REQUEST,
    WRITING_RESPONSE,
    DONE
}	t_client_state;

class Config;
class Request;

class Client
{
    public:
        Client();
        Client(int fd, const Config* config);
        Client(const Client& to_copy);
        Client& operator=(const Client& to_copy);
        ~Client();

        int                 clientFd;
        int                 clientState;

        std::vector<char>   writeBuff;
        size_t              bytesSent;
        size_t              buffSize;

        time_t              _lastActivity;

        Request             _request;
        const Config* 		_serverConfig;
        bool                _keepAlive;

        int                 timeout();
        void                updateActivity();
        time_t              getLastActivity() const;
        Request&            getRequest();
        const Config&       getConfig() const;
        int                 getFd() const;
        void                resetClient();
};

#endif