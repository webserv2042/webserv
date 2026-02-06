#include "../includes/server/Server.hpp"

int main (void)
{
	Server	server(LISTENING_PORT);

	try
	{
		//PARTIE MARIO
		std::cout << "Initalizing server..." << std::endl;
		server.init();

		// std::cout << "Tentative d'accept..." << std::endl;
		// int client = accept(server.socketFD, NULL, NULL);
		// if (client == -1)
		// 	std::cout << "Succes, cest non bloquant" << std::endl;
		// else
		// 	std::cout << "Fail cest bloquant" << std::endl;

		//PARTIE POLL
		// std::cout << "Entering main loop..." << std::endl;

		//TEST
		std::cout << "Epoll test" << std::endl;

		//create epoll instance
		int	ep_fd = epoll_create(1);

		//set the event struct to the right event + fd
		struct epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = server.socketFD;

		//add your socket_fd to the list of fds to monitor
		epoll_ctl(ep_fd, EPOLL_CTL_ADD, server.socketFD, &event);

		//wait for an event to occur
		struct epoll_event events[MAX_EVENTS]; //where the kernel will give the info of the event/fd back
		int ready_fds = 0;
		ready_fds = epoll_wait(ep_fd, events, MAX_EVENTS, 5000);

		//loop through the occured events
		int client_fd = -1;
		std::cout << "fd ready: " << ready_fds << std::endl;
		for (int i = 0; i < ready_fds; i++)
		{
			std::cout << "EVENT N" << i <<  std::endl;
			std::cout << events[i].events << std::endl;
			std::cout << events[i].data.fd << std::endl;

			//accept the connection and stock it in client_fd
			client_fd = accept(events[i].data.fd, NULL, NULL);
			//repeat the previous process to add the client fd to watchlist
			struct epoll_event cl_event;
			cl_event.events = EPOLLIN;
			cl_event.data.fd = client_fd;
			epoll_ctl(ep_fd, EPOLL_CTL_ADD, client_fd, &cl_event);

			//wait
			struct epoll_event events2[MAX_EVENTS];
			int nready = 0;
			nready = epoll_wait(ep_fd, events2, MAX_EVENTS, 5000);
			//if theres something, read it
			if (nready > 0)
			{
				std::cout << "ACCEPTED FD EVENT:" << std::endl;
				std::cout << events2[0].events << std::endl;
				std::cout << events2[0].data.fd << std::endl;

				char buffer[2000];
				read(events2[0].data.fd, buffer, sizeof(buffer));
				std::cout << std::endl;
				std::cout << "Content:" << std::endl;
				std::cout << buffer << std::endl;
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}



	// Fermeture des sockets et libération des ressources
	close(server.socketFD);
	// close(socketFD);

	return (0);
}
