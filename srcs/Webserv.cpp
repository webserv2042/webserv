#include "../include/Webserv.hpp"
#include "../include/Client.hpp"
#include "../include/Signals.hpp"
#include "../include/Request.hpp"
#include "../include/Response.hpp"

//CONSTRUCTOR(S)
Webserv::Webserv() {
    ep_fd = -1;
	ready_fds = -1;
}

//DESTRUCTOR
Webserv::~Webserv() {
}

//PUBLIC

/// @brief boucle principale epoll : orchestre l'attente et la reception de connections
void    Webserv::epollLoop()
{
	startEpoll();

	while (server_running)
	{
		//epoll_wait attend de recevoir des connections
		std::cout << "\033[90mWAITING...\033[0m" << std::endl;
        if (waitForEvents() == SIGNAL_RECEIVED)
            break;
		
		checkIdleTimeout();

		//connection(s) trouvee(s) -> parcourir les events
		for (int i = 0; i < ready_fds; i++)
		{
			int fd = events[i].data.fd;

			if (isSocketFd(fd) == true) //nouvelle connexion sur une socket -> accepter
				acceptClient(fd);

			else //requete/reponse client -> lire/ecrire
			{
				//deconnexion client
				if (events[i].events & (EPOLLHUP | EPOLLERR))
				{
					std::cout << "\033[31mDISCONNECT !!!!\033[0m" << std::endl;
					epoll_ctl(ep_fd, EPOLL_CTL_DEL, fd, NULL);
					clients[fd].~Client();
					close(fd);
				}

				//lire la requete, la parser, preparer la reponse etc...
				else if (events[i].events == EPOLLIN)
					treatRequest(fd);

				//renvoyer la reponse
				else if (events[i].events == EPOLLOUT)
					sendResponse(clients[fd]);
			}
			std::cout << "--------------------------" << std::endl;
		}
	}

	std::cout << "Server stopped." << std::endl;
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
	char    buffer[4096];
	ssize_t bytesReceived;

	bytesReceived = recv(fd, buffer, sizeof(buffer), 0);

	if (bytesReceived > 0)
	{
		_client[fd].updateActivity();
		_client[fd].getRequest().feeding(buffer, (size_t)bytesReceived); // on récup le client du fd nommé, on copie les octets reçus du buffer vers sa requête
		if (_client[fd].getRequest().isFinished())
		{
			std::cout << "Requête terminée du fd " << fd << " !" << std::endl;

			const Config		&config = _client[fd].getConfig();
			Response			res(config);

			res.setResponseFinal(_client[fd].getRequest());
			_client[fd]._keepAlive = !res.getCloseFd();

			std::vector<char>	responseToSend = res.getResponseFinal();

			_client[fd].writeBuff.assign(responseToSend.begin(), responseToSend.end());
            _client[fd].buffSize = _client[fd].writeBuff.size();
            _client[fd].bytesSent = 0;

			_client[fd].clientState = WRITING_RESPONSE;
            modifyEpollout(fd, ADD_EPOLLOUT);
		}
	}
	else
		closeClient(fd);
}

/// @brief fonction pour tester de lire a partir du fd client (a supp + tard)
/// @param clientFD fd du client
int	Webserv::testRead(int &clientFD)
{
	clients[clientFD].clientState = READING_REQUEST;
	char buffer[2000];

	int bytesRead = read(clientFD, buffer, sizeof(buffer));
	if (bytesRead == ERROR || bytesRead == 0) //deconnexion client
	{
		closeClient(clientFD);
		return DISCONNECTION;
	}

	std::cout << std::endl << "Content:" << std::endl;
	buffer[bytesRead] = '\0';
	std::cout << buffer << std::endl;
	memset(buffer, 0, 2000);

	//rempli la reponse temporairement
	// clients[clientFD].writeBuff = "test response: hi!\n";
	// clients[clientFD].buffSize = clients[clientFD].writeBuff.size();


	//test avec bcp de data
	// std::string bigBody(5 * 1024 * 1024, 'A'); // 5 MB of 'A'
	// std::string str;
	// std::stringstream ss;
	// ss << bigBody.size();
	// ss >> str;
	// clients[clientFD].writeBuff =
    // "HTTP/1.1 200 OK\r\n"
    // "Content-Length: " + str + "\r\n"
    // "Content-Type: text/plain\r\n"
    // "Connection: close\r\n"
    // "\r\n" +
    // bigBody + "\n--END--\n";
	// clients[clientFD].buffSize = clients[clientFD].writeBuff.size();

	//test avec une vraie reponse http
	std::string body = readFile("./www/index.html");

	std::string str;
	std::stringstream ss;
	ss << body.size();
	ss >> str;

	clients[clientFD].writeBuff =
	"HTTP/1.1 200 OK\r\n"
    "Date: Wed, 25 Feb 2026 16:14:00 GMT\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: " + str + "\r\n"
    "Connection: close\r\n"
    "\r\n" +
	body;

	clients[clientFD].buffSize = clients[clientFD].writeBuff.size();
	
	clients[clientFD].clientState = PROCESSING;
	return 0;
}

/// @brief fonction qui renvoi une reponse au client en non-bloquant(=potentiellement en plusieurs fois)
/// @param client le client a qui on veut renvoyer la reponse
void	Webserv::sendResponse(Client &client)
{
	std::cout << "\033[34mSENDING RESPONSE...\033[0m" << std::endl;
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
			if (errno == EAGAIN || errno == EWOULDBLOCK) //mode non bloquant active, le renvoi sera reessaye
				return;
			else //erreur d'envoi -> client supprime
			{
				std::cerr << "(SERVER) couldn't send response" << std::endl;
				closeClient(client.clientFd);
				return;
			}
		}
		else //ajout des bytes envoyés au compte
			client.bytesSent += currentBytes;
	}

	if (client.bytesSent == client.buffSize) // tous les bytes envoyes? ->client retourne en EPOLLIN, sinon retourne ds la boucle d'envoi
    {
        if (client._keepAlive == false) // si ma réponse a dit de fermer selon headers
        {
            std::cout << "Closing connection as requested by Response headers." << std::endl;
            closeClient(client.clientFd);
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
        if (servers[i].getSocketFD() != -1)
            close(servers[i].getSocketFD());
        servers[i].setSocketFD(-1);
    }

    //close epoll instance
    if (ep_fd != -1)
        close(ep_fd);
    ep_fd = -1;
}

//gerer le timeout OK
//gerer les gros buffers OK
//gerer pollout OK
//gerer les signaux OK
//gerer les leaks/fd OK
//gerer plusieurs servers en mm temps OK
