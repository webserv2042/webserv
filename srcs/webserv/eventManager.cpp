#include "../../includes/Webserv.hpp"
#include "../../includes/server/Client.hpp"
#include "../../includes/Signals.hpp"
#include "../../includes/http/Response.hpp"

//* FICHIER POUR LES FONCTIONS UTILISEES DANS LA BOUCLE EPOLL

//PRIVATE

/// @brief Cree une nouvelle instance epoll et surveille les socketFD(=ports) de chaque server
void    Webserv::startEpoll()
{
	//cree une instance epoll
	ep_fd = epoll_create(1);
	if (ep_fd == ERROR)
		throw std::runtime_error("(SERVER) failed to create epoll instance: epoll_create: ");
    
    for (long unsigned int i = 0; i < servers.size(); i++)
	    registerNewFd(servers[i].getSocketFD(), EPOLLIN);
}

/// @brief epoll_wait attend de recevoir des connections
/// @return 1 si un signal interrompt, 0 si tt va bien
int    Webserv::waitForEvents()
{
    ready_fds = epoll_wait(ep_fd, events, MAX_EVENTS, 30 * 1000);
    if (ready_fds == ERROR)
    {
        if (errno == EINTR) //signal interruption -> stop la boucle et libere tout
            return 1;
        else
            throw std::runtime_error("(SERVER) epoll_wait function failed: ");
    }
    return 0;
}

/// @brief accepte une connection client et l'enregistre dans la liste des clients (clients)
/// @param newConnexionFd le fd sur lequel la nouvelle connexion a ete etablie
void    Webserv::acceptClient(int &newConnexionFd)
{
	// std::cout << "\033[32mFOUND CONNECTION ->>\033[0m" << std::endl;
	int client_fd = -1;

	try
	{
		client_fd = accept(newConnexionFd, NULL, NULL);
		if (client_fd == ERROR)
			throw std::runtime_error("(SERVER) Failed to accept connection: ");
		
		setNonBlockingSocket(client_fd);
		registerNewFd(client_fd, EPOLLIN);

		// AJOUT DE SHEINEZ POUR AJOUTER LA CONFIG A LA CREATION DE CHAQUE CLIENT
		const Config* foundConfig = NULL;
		for (size_t i = 0; i < servers.size(); i++)
		{
			if (servers[i].getSocketFD() == newConnexionFd)
			{
				foundConfig = &(servers[i].getConfig());
				break;
			}
		}

		// CREATE NEW CLIENT //
		Client newClient(client_fd,foundConfig, ep_fd);
		clients[client_fd] = newClient; //do a proper copy!!!!
		std::cout << "accepted : " << client_fd << std::endl;
	}
	catch(const std::exception& e)
	{
		if (client_fd > 0)
			closeClient(client_fd);
		std::cerr << e.what();
		std::cerr << strerror(errno) << std::endl;
	}
}


/// @brief ferme une instance client: supprime le client de la liste, arrete la surveillance
/// de son fd, et ferme le fd 
/// ->appeler cette fonction en cas d'erreur ou de deconnexion du client
/// @param clientFD fd du client a supprimer
void    Webserv::closeClient(int clientFD)
{
	std::cout << "\033[31mDISCONNECTING \033[0m" << clientFD << std::endl;
    epoll_ctl(ep_fd, EPOLL_CTL_DEL, clientFD, NULL);
    close(clientFD);
	clients.erase(clientFD);
}

/// @brief ferme une instance client: supprime le client de la liste, arrete la surveillance
/// de son fd, et ferme le fd 
/// @param it iterateur indiquant la position du client a supprimer
/// @return la position suivante de l'iterateur
std::map<int, Client>::iterator    Webserv::closeClient(std::map<int, Client>::iterator it)
{
	std::cout << "\033[31mDISCONNECT -it- ON \033[0m" << it->first << std::endl;
    epoll_ctl(ep_fd, EPOLL_CTL_DEL, it->first, NULL);
    close(it->first);

	std::map<int, Client>::iterator next = it;
    ++next;
	clients.erase(it);

	return next;
}

/// @brief verifie que le timeout d'aucun client n'a ete depasse, et deconnecte les clients
/// en consequence
void    Webserv::checkIdleTimeout()
{
	std::cout << "----------IN TIMEOUT-----------" << std::endl;

	// -- PHASE 1 : collecter les fd en timeout -- //
	std::vector<int> cgiTimeouts;     // fd des pipes CGI en timeout
	std::vector<int> clientTimeouts;  // fd des clients normaux en timeout

	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.timeout() == TIMEOUT)
		{
			if (it->second.isCGI == IS_CGI)
				cgiTimeouts.push_back(it->first);
			else
				clientTimeouts.push_back(it->first);
		}
	}

	// -- PHASE 2a : traiter les pipes CGI en timeout -- //
	for (size_t i = 0; i < cgiTimeouts.size(); i++)
	{
		std::cout << "\033[31m TIMEOUT ON CGI \033[0m" << cgiTimeouts[i] << std::endl;
		int cgiFd = cgiTimeouts[i];

		// le pipe a pu etre ferme entre temps par un autre traitement
		if (clients.find(cgiFd) == clients.end())
			continue;

		int targetFd = clients[cgiFd].ogFd;

		// kill le child process
		if (clients[cgiFd].forkPid > 0)
		{
			kill(clients[cgiFd].forkPid, SIGKILL);
			waitpid(clients[cgiFd].forkPid, NULL, WNOHANG);
		}

		// fermer le pipe CGI
		closeClient(cgiFd);

		// envoyer 504 au client original
		if (clients.find(targetFd) != clients.end())
			clients[targetFd].clientState = WAITING_FOR_CGI;
	}

	// -- PHASE 2b : traiter les clients normaux en timeout -- //
	for (size_t i = 0; i < clientTimeouts.size(); i++)
	{
		int fd = clientTimeouts[i];

		// le client a pu etre ferme entre temps (ex: etait le targetFd d'un CGI ci-dessus)
		if (clients.find(fd) == clients.end())
			continue;

		try
		{
			// FIX: si le client attend un CGI -> 504 Gateway Timeout
			//      sinon -> 408 Request Timeout
			if (clients[fd].clientState == WAITING_FOR_CGI)
			{
				std::cout << "WAITING ON CGI" << std::endl;
				clients[fd].getRequest().fail(GATEWAY_TIMEOUT);
				clients[fd].updateActivity();
			}
			else if (clients[fd].clientState == WRITING_RESPONSE)
			{
				std::cout << "NOT WAITING ON CGI" << std::endl;
				clients[fd].getRequest().fail(REQUEST_TIMEOUT);
				clients[fd].updateActivity();
			}
			else
			{
				std::cout << "NEITHER" << std::endl;
				closeClient(fd);
			}
		}
		catch (const std::exception &e)
		{
			writeResponse(fd);
		}
	}

	std::cout << "----------OUT OF TIMEOUT-----------" << std::endl;
}