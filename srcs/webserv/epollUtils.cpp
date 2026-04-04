#include "../../includes/Webserv.hpp"
#include "../../includes/server/Client.hpp"
#include "../../includes/Signals.hpp"
#include <fstream>
#include <sstream>
#include <string>


/// @brief rend un fd de socket non-bloquant
/// @param fdSocket le fd a rendre non-bloquant
void    Webserv::setNonBlockingSocket(int &fdSocket)
{
	int flags = fcntl(fdSocket, F_GETFL);
	if (flags == ERROR)
		throw std::runtime_error("(SERVER) Failed to get socket configuration: ");
	flags = flags | O_NONBLOCK;

	fcntl(fdSocket, F_SETFL, flags);
}

/// @brief ajoute un nouveau fd a la liste de surveillance selon l'evenement precise
/// @param newFd le fd a surveiller
/// @param epEvent l'evenement a surveiller (ex: EPOLLIN, EPOLLOUT...)
void    Webserv::registerNewFd(const int &newFd, uint32_t epEvent)
{
	//definir l'evenement que l'on veut surveiller(nouvelles connections) + sur quel fd
	struct epoll_event event;
	event.events = epEvent;
	event.data.fd = newFd;

	//ajouter le fd de notre socket a la liste de surveillance
	if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, newFd, &event) == ERROR)
		throw std::runtime_error("(SERVER) epoll_ctl function failed: ");
}

/// @brief check si le fd est un socket ou un client
/// @param sockFD fd a verifier
/// @return true/false
bool    Webserv::isSocketFd(int &sockFD)
{
    for (long unsigned int i = 0; i < servers.size(); i++)
    {
        if (sockFD == servers[i].getSocketFD())
            return true;
    }
    return false;
}

/// @brief change l'event surveille d'un fd pour y ajouter/enlever EPOLLOUT selon l'action
/// @param fd le fd a modifier
/// @param action ADD_EPOLLOUT(0) = ajouter epollout DELETE_EPOLLOUT(1) enlever epollout
void    Webserv::modifyEpollout(int &fd, int action)
{
	//definir l'evenement que l'on veut surveiller(EPOLLOUT pour pouvoir ecrire, EPOLLIN pour rester en keep-alive)
    // + sur quel fd
	struct epoll_event event;
    if (action == ADD_EPOLLOUT)
	    event.events = EPOLLIN | EPOLLOUT;
    if (action == DELETE_EPOLLOUT)
        event.events = EPOLLIN;
    event.data.fd = fd;
    
	//modifier la surveillance du fd avec epoll_ctl
	if (epoll_ctl(ep_fd, EPOLL_CTL_MOD, fd, &event) == ERROR)
		throw std::runtime_error("MODEPOLL (SERVER) epoll_ctl function failed: ");
}

std::string readFile(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
	{
		// std::cout << "ERROR FILENAME" << std::endl;
		return ""; // or handle error differently
	}

    std::ostringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}