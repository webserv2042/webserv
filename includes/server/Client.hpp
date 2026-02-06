#ifndef CLIENT_HPP

# define CLIENT_HPP

#include <string.h>
#include <iostream>
#include <stdlib.h>

class Client {
    public:
        Client();
        Client(const Client& to_copy);
        Client& operator=(const Client& to_copy);
        ~Client();

    private:
        /*content*/
    
};

#endif