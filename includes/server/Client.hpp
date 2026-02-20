#ifndef CLIENT_HPP

# define CLIENT_HPP

#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <ctime>

#define NO_TIMEOUT 0
#define TIMEOUT 1

typedef enum t_client_state
{
    CONNEXION_ACCEPTED,
	READING_REQUEST,
	PROCESSING,
	WRITING_RESPONSE,
	DONE
}	t_client_state;

class Client {
    public:
        Client();
        Client(int new_client_fd);
        Client(const Client& to_copy);
        Client& operator=(const Client& to_copy);
        ~Client();

        int         clientFd;
        int         clientState;
        std::string readBuff;

        //renvoi de reponse
        std::string writeBuff;
        int         bytesSent; //nb de bytes envoyés au client
        int         buffSize; //taille totale du writeBuff

        //timeout
        time_t      lastActivity;
        // time_t      totalConnection;
        // time_t      readTime;
        // time_t      writeTime;
        int         Timeout();

        void        resetClient();

    private:
        /*content*/
    
};

#endif