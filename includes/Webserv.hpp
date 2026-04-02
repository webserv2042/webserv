#ifndef WEBSERV_HPP

# define WEBSERV_HPP

#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>

#include <sys/epoll.h>
#include <stdexcept>
#include <errno.h>
#include <map>
#include <vector>
#include <sstream>
#include <ctime>

#include <sys/wait.h>	
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server/Client.hpp"
#include "server/Server.hpp"


#define MAX_EVENTS 124 //nombre d'evenements enregistres a la fois par epoll_wait

// RETOURS FONCTIONS
#define ERROR -1
#define SIGNAL_RECEIVED 1
#define DISCONNECTION 2
#define ADD_EPOLLOUT 0
#define DELETE_EPOLLOUT 1

// PIPE TYPES
#define PIPE_IN 1
#define PIPE_OUT 2

//TIMEOUT
// #define IDLE_TIMEOUT 60 //timeout d'un client sans aucune activité
// #define NO_TIMEOUT 0
// #define TIMEOUT 1
// #define TOTAL_TIMEOUT 300 //timeout de la connection totale d'un client
// #define READ_TIMEOUT 10 //timeout de reception de requete
// #define WRITE_TIMEOUT 600 //timeout d'envoi de reponse

/// @brief classe generale qui contient le(s) serveur(s), les clients, et orchestre la boucle epoll
class Webserv {
    public:
        Webserv();
        ~Webserv();

		//* FONCTIONS PRINCIPALES

		void								setServers(std::vector<Server> &servVec);
		void 								epollLoop();
		void								finalClean();

		std::map<int, Client>				clients; //liste des clients

    private:

		//* EVENT MANAGER

		int									ep_fd; //instance epoll
		std::vector<Server>					servers; //liste des serveurs
		struct epoll_event					events[MAX_EVENTS]; //tableau des evenements enregistres par epoll_wait
		int									ready_fds; //nombre de fd ayant recus des evenements

		void 								startEpoll();
		int									waitForEvents();
		void								acceptClient(int &newConnexionFd);
		void								closeClient(int clientFD);
		std::map<int, Client>::iterator		closeClient(std::map<int, Client>::iterator it);
		void								checkIdleTimeout();

		//* EPOLL UTILS

		void 								setNonBlockingSocket(int &fdSocket);
		void								registerNewFd(const int &newFd, uint32_t event);
		bool								isSocketFd(int &sockFD);
		void								modifyEpollout(int &fd, int action);


		//* REQUETE + REPONSE

    	void 			                  	treatRequest(int &fd);
		void								writeResponse(int fd);
		void								CGIwriteToChild(int fd);
		void								CGIreadFromChild(int fd);
		void								CGIprepareResponse(int fd, std::vector<char> cgiOutput);
		void								sendResponse(Client &client);

};

//fonction temporaire pour generer une reponse http a partir d'un fichier
std::string readFile(const std::string& filename);

#endif