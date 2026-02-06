#include "../../includes/server/Server.hpp"

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

/// @brief Cree les elements necessaires pour la boucle epoll
void Server::startEpoll()
{
	//cree une instance epoll
	this->ep_fd = epoll_create(1); //close later
	if (ep_fd == ERROR)
		throw std::runtime_error("Epoll creation failed");

	//definir l'evenement que l'on veut surveiller(nouvelles connections) + sur quel fd(notre socket)
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = this->socketFD;

	//ajouter le fd de notre socket a la liste de surveillance
	if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, this->socketFD, &event) == ERROR)
		throw std::runtime_error("epoll_ctl function failed");
}

/// @brief boucle principale epoll
void Server::epollLoop()
{
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

			if (fd == this->socketFD) //nouvelle connexion -> accepter
			{
				//accept
			}
			else //requete client -> lire/ecrire
			{
				//read/write
			}
		}
	}
}