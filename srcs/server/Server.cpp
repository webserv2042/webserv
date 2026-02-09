#include "../../includes/server/Server.hpp"
#include "../../includes/server/Client.hpp"

Server::Server(int port)
{
	this->port = port;
	this->socketFD = -1;
}

void Server::init()
{
	this->socketFD = socket(IPV4, SOCK_STREAM, 0);
	if (this->socketFD == -1)
		throw std::runtime_error("(SERVER) Failed to initialize socket");
	int flags = fcntl(this->socketFD, F_GETFL);
	if (flags == -1)
		throw std::runtime_error("(SERVER) Failed to get socket configuration");
	flags = flags | O_NONBLOCK;
	fcntl(this->socketFD, F_SETFL, flags);
	this->socketAddress.sin_family = IPV4;
	this->socketAddress.sin_port = htons(LISTENING_PORT); // htons = host to network short
	this->socketAddress.sin_addr.s_addr = htonl(INADDR_ANY); // htonl = host to network long
	this->socketAddressLength = sizeof(this->socketAddress);
	int bindReturnCode = bind(socketFD, (struct sockaddr*) &this->socketAddress, this->socketAddressLength);
	if (bindReturnCode == -1)
		throw std::runtime_error("(SERVER) Failed to link socket");
	int listenReturnCode = listen(socketFD, PENDING_QUEUE_MAXLENGTH);
	if (listenReturnCode == -1)
		throw std::runtime_error("(SERVER) Failed to start listening");
}

//daniya

/// @brief Cree une nouvelle instance epoll et surveille son fd
void Server::startEpoll()
{
	//cree une instance epoll
	ep_fd = epoll_create(1); //close later
	if (ep_fd == ERROR)
		throw std::runtime_error("Epoll creation failed");

	registerNewFd(socketFD, EPOLLIN);
}

/// @brief boucle principale epoll
void Server::epollLoop()
{
	startEpoll();
	
	struct epoll_event events[MAX_EVENTS];
	int	ready_fds = 0;

	while (SERVER_RUNNING)
	{
		//epoll attend de recevoir des connections
		ready_fds = epoll_wait(this->ep_fd, events, MAX_EVENTS, -1);
		if (ready_fds == ERROR)
			throw std::runtime_error("epoll_wait function failed");
		
		//connection trouvee
		for (int i = 0; i < ready_fds; i++)
		{
			int fd = events[i].data.fd;

			if (fd == socketFD) //nouvelle connexion -> accepter
			{
				acceptClient(fd);
			}
			else //requete client -> lire/ecrire
			{
				if (events[i].events == EPOLLHUP)
				{
					epoll_ctl(ep_fd, EPOLL_CTL_DEL, fd, NULL);
					close(fd);
				}
				//read/write
				char buffer[2000];
				int bytesRead = read(events[i].data.fd, buffer, sizeof(buffer));
				if (bytesRead == 0 || bytesRead == ERROR)
				{
					epoll_ctl(ep_fd, EPOLL_CTL_DEL, fd, NULL);
					clientMap[fd].~Client();
					close(fd);
				}
				else
				{
					std::cout << std::endl;
					std::cout << "Content:" << std::endl;
					std::cout << buffer << std::endl;
					memset(buffer, 0, 2000);
				}
			}
		}
	}
}

/// @brief accepte une connection client et l'enregistre dans la liste des clients (clientMap)
/// @param newConnexionFd le fd sur lequel la nouvelle connexion a ete etablie
void Server::acceptClient(int newConnexionFd)
{
	int client_fd = -1;

	client_fd = accept(newConnexionFd, NULL, NULL);
	if (client_fd == ERROR)
		throw std::runtime_error("(SERVER) Failed to accept connection");
	
	setNonBlockingSocket(client_fd);
	registerNewFd(client_fd, EPOLLIN);

	Client newClient(client_fd);
	clientMap[client_fd] = newClient;
}

/// @brief rend un fd de socket non-bloquant
/// @param fdSocket le fd a rendre non-bloquant
void Server::setNonBlockingSocket(int fdSocket)
{
	int flags = fcntl(fdSocket, F_GETFL);
	if (flags == ERROR)
		throw std::runtime_error("(SERVER) Failed to get socket configuration");
	flags = flags | O_NONBLOCK;

	fcntl(fdSocket, F_SETFL, flags);
}

/// @brief ajoute un nouveau fd a la liste de surveillance selon l'evenement precise
/// @param newFd le fd a surveiller
/// @param epEvent l'evenement a surveiller (ex: EPOLLIN, EPOLLOUT...)
void Server::registerNewFd(int newFd, uint32_t epEvent)
{
	//definir l'evenement que l'on veut surveiller(nouvelles connections) + sur quel fd(notre socket)
	struct epoll_event event;
	event.events = epEvent;
	event.data.fd = newFd;

	//ajouter le fd de notre socket a la liste de surveillance
	if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, newFd, &event) == ERROR)
		throw std::runtime_error("(SERVER) epoll_ctl function failed");
}