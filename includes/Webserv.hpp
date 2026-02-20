#ifndef WEBSERV_HPP

# define WEBSERV_HPP

#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <poll.h>
#include <sys/epoll.h>
#include <stdexcept>
#include <errno.h>
#include <map>
#include <vector>
#include <sstream>
#include <ctime>

#include "server/Client.hpp"
#include "server/Server.hpp"


#define MAX_EVENTS 3 //nombre d'evenements enregistres a la fois par epoll_wait

// RETOURS FONCTIONS
#define ERROR -1
#define SIGNAL_RECEIVED 1
#define DISCONNECTION 2
#define ADD_EPOLLOUT 0
#define DELETE_EPOLLOUT 1

//TIMEOUT
#define IDLE_TIMEOUT 60 //timeout d'un client sans aucune activité
// #define TOTAL_TIMEOUT 300 //timeout de la connection totale d'un client
// #define READ_TIMEOUT 10 //timeout de reception de requete
// #define WRITE_TIMEOUT 600 //timeout d'envoi de reponse

/// @brief classe generale qui contient le(s) serveur(s), les clients, et orchestre la boucle epoll
class Webserv {
    public:
        Webserv();
        ~Webserv();

		//* FONCTIONS PRINCIPALES

		void					setServer(Server &server);
		void 					epollLoop();
		void					finalClean();

		std::map<int, Client>	clients; //liste des clients

    private:

		//* EVENT MANAGER

		int						ep_fd; //instance epoll
		std::vector<Server>		servers; //liste des serveurs
		struct epoll_event		events[MAX_EVENTS]; //tableau des evenements enregistres par epoll_wait
		int						ready_fds; //nombre de fd ayant recus des evenements

		void 					startEpoll();
		int						waitForEvents();
		void					acceptClient(int &newConnexionFd);
		void					closeClient(int clientFD);
		std::map<int, Client>::iterator    closeClient(std::map<int, Client>::iterator it);
		void					checkIdleTimeout();

		//* EPOLL UTILS

		void 					setNonBlockingSocket(int &fdSocket);
		void					registerNewFd(int &newFd, uint32_t event);
		bool					isSocketFd(int &sockFD);
		void					modifyEpollout(int &fd, int action);


		//* REQUETE + REPONSE

		void					treatRequest(int &fd);
		int						testRead(int &clientFD); //supp + tard
		void					sendResponse(Client &client);
    
};

#endif