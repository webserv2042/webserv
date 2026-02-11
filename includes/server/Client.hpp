#ifndef CLIENT_HPP

# define CLIENT_HPP

#include <string.h>
#include <iostream>
#include <stdlib.h>

typedef enum t_client_state
{
    REQUEST_RECEIVED,
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
        std::string writeBuff;

    private:
        /*content*/
    
};

#endif