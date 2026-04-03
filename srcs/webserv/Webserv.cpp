#include "../../includes/Webserv.hpp"
#include "../../includes/server/Client.hpp"
#include "../../includes/Signals.hpp"
#include "../../includes/http/Response.hpp"
#include <cstdio>

//CONSTRUCTOR(S)
Webserv::Webserv() {
    ep_fd = -1;
	ready_fds = -1;
}

//DESTRUCTOR
Webserv::~Webserv() {
}

/// @brief boucle principale epoll : orchestre l'attente et la reception de connections
void    Webserv::epollLoop()
{
	startEpoll();

	while (server_running)
	{
		//epoll_wait attend de recevoir des connections
		// std::cout << "\033[90mWAITING...\033[0m" << std::endl;
        if (waitForEvents() == SIGNAL_RECEIVED)
            break;
		
		checkIdleTimeout();

		//connection(s) trouvee(s) -> parcourir les events
		for (int i = 0; i < ready_fds; i++)
		{
			int fd = events[i].data.fd;
			// std::cout << "connexion on fd n°" << fd << std::endl;

			if (isSocketFd(fd) == true) //nouvelle connexion sur une socket -> accepter
				acceptClient(fd);

			else if (clients[fd].isCGI == IS_CGI) // traitement CGI
			{
				if (events[i].events & EPOLLHUP && clients[fd].clientState == READING_CGI)
					CGIreadFromChild(fd);
				if (events[i].events & EPOLLIN)
					CGIreadFromChild(fd);
				if (events[i].events & EPOLLOUT)
					CGIwriteToChild(fd);
			}

			else //requete/reponse client -> lire/ecrire
			{
				//deconnexion client
				if (events[i].events & (EPOLLHUP | EPOLLERR))
				{
					closeClient(fd);
					continue ; // AJOUTE ICI A VOIR QUOI EN FAIRE
				}

				//lire la requete, la parser, preparer la reponse,...
				if (events[i].events & EPOLLIN)
					treatRequest(fd);

				//envoyer la reponse
				if (events[i].events & EPOLLOUT)
					sendResponse(clients[fd]);
			}
		}
	}

	// std::cout << "Server stopped." << std::endl;
    finalClean();
}

/// @brief ajoute le vecteur de serveurs a webserv
/// @param servVec les serveurs a ajouter
void    Webserv::setServers(std::vector<Server> &servVec)
{
	servers = servVec;
	for (size_t i = 0; i < servers.size(); i++)
		servers[i].init();
}

//PRIVATE

/// @brief lis et traite la requete d'un client
/// @param fd fd du client
void	Webserv::treatRequest(int &fd)
{
	// READ REQUEST //
	char    buffer[4096];
	ssize_t bytesReceived;

	bytesReceived = recv(fd, buffer, sizeof(buffer), 0);
	if (bytesReceived > 0)
	{
		clients[fd].updateActivity();
		clients[fd].getRequest().feeding(buffer, (size_t)bytesReceived); // on récup le client du fd nommé, on copie les octets reçus du buffer vers sa requête
		// DONE READING //
		if (clients[fd].getRequest().isFinished())
		{
			// PRINT REQUEST //
			// std::cout << "\033[38;5;211mrequest-complete !\033[0m" << std::endl;
        	// clients[fd].getRequest().printRequest();
			// std::cout << "\033[38;5;211m-----------request-end--------\033[0m" << std::endl;

			// RESPONSE //
			writeResponse(fd);
		}
	}
	else
		closeClient(fd);
}

void	Webserv::writeResponse(int &fd)
{
	const Config		&config = clients[fd].getConfig();
	Response			res(config);

	if (clients[fd]._requestCount >= MAX_KEEPALIVE_REQUESTS)
    	clients[fd]._keepAlive = false;
	// GET RESPONSE OR START CGI //
	if (res.setResponseFinal(clients[fd].getRequest(), fd, clients) == IS_CGI)
		return;
	
	// FINISH RESPONSE
	clients[fd]._keepAlive = !res.getCloseFd();

	std::vector<char>	responseToSend = res.getResponseFinal();

	clients[fd].writeBuff.assign(responseToSend.begin(), responseToSend.end());
	clients[fd].buffSize = clients[fd].writeBuff.size();
	clients[fd].bytesSent = 0;

	clients[fd].clientState = WRITING_RESPONSE;
	modifyEpollout(fd, ADD_EPOLLOUT);
}



/// @brief fonction qui renvoi une reponse au client en non-bloquant(=potentiellement en plusieurs fois)
/// @param client le client a qui on veut renvoyer la reponse
void	Webserv::sendResponse(Client &client)
{
	// PRINT RESPONSE //
	// std::cout << "\033[38;5;117m-----------response------------\033[0m" << std::endl;
	// std::string s(client.writeBuff.begin(), client.writeBuff.end());
	// std::cout << s << std::endl;
	// std::cout << "\033[38;5;117m-----------response------------\033[0m" << std::endl;
	client.clientState = WRITING_RESPONSE;

	//Calcule le nombre de bytes restants a envoyer
	int currentBytes = 0;
	int bytesLeft = client.buffSize - client.bytesSent;

	//s'il y en a encore, les envoyer
	if (bytesLeft > 0)
	{
		currentBytes = send(client.clientFd, &client.writeBuff[client.bytesSent], bytesLeft, MSG_DONTWAIT);
		if (currentBytes <= 0)
		{
			// if (errno == EAGAIN || errno == EWOULDBLOCK) //mode non bloquant active, le renvoi sera reessaye
			// 	return;
			//erreur d'envoi -> client supprime
			std::cerr << "(SERVER) couldn't send response" << std::endl;
			closeClient(client.clientFd);
			return;
		}
		else //ajout des bytes envoyés au compte
			client.bytesSent += currentBytes;
	}

	if (client.bytesSent == client.buffSize) // tous les bytes envoyes? ->client retourne en EPOLLIN, sinon retourne ds la boucle d'envoi
    {
        if (client._keepAlive == false) // si ma réponse a dit de fermer selon header Connection
        {
            // std::cout << "Closing connection as requested by Response headers." << std::endl;
            closeClient(client.clientFd);
			return ;
        }
        else // sinon on reste ouvert
        {
            client.resetClient();
            modifyEpollout(client.clientFd, DELETE_EPOLLOUT);
            client.clientState = READING_REQUEST;
        }
    }
    client._lastActivity = time(NULL);
}


/// @brief ferme et supprime les donnees necessaires a la fin de la boucle ->
/// clients, server sockets, epoll instance
void    Webserv::finalClean()
{
    //close clients fd + whole client list
    std::map<int, Client>::iterator it;

    for (it = clients.begin(); it != clients.end(); it++)
    {
        if (it->first != -1)
            close(it->first);
    }
    clients.clear();

    //close server socketFD
    for (long unsigned int i = 0; i < servers.size(); i++)
    {
        if (servers[i].getSocketFD() != -1) {
			std::cout << servers[i].getSocketFD() << " is closed." << std::endl;
            close(servers[i].getSocketFD());
		}
        servers[i].setSocketFD(-1);
    }

    //close epoll instance
    if (ep_fd != -1)
        close(ep_fd);
    ep_fd = -1;
}
