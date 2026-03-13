#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "request.hpp"
#include <ctime>

class Client
{
	private:
		int     _fd;
		Request _request; // chaque client possède sa propre requête, ainsi tout est indépendant et individuel, plus facile à traiter si erreur
		time_t  _lastActivity;

	public:
		Client(int fd = 0);
		~Client();

		int         getFd() const;
		Request&    getRequest();

		void		updateActivity();
		time_t		getLastActivity() const; // pour que la boucle epoll connaisse l'état d'activité du client
};

#endif