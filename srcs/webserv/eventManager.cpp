#include "../../includes/Webserv.hpp"
#include "../../includes/server/Client.hpp"
#include "../../includes/Signals.hpp"

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
	std::cout << "\033[32mFOUND CONNECTION ->>\033[0m" << std::endl;
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
	std::cout << "\033[31mDISCONNECT\033[0m" << std::endl;
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
	std::cout << "\033[31mDISCONNECT !!!!\033[0m" << std::endl;
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
    std::map<int, Client>::iterator it = clients.begin();

	while (it != clients.end())
	{
		if (it->second.timeout() == TIMEOUT)
			it = closeClient(it);
		else
			++it;
	}
}