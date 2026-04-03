#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../http/Request.hpp"
#include "../config/Config.hpp"
#include <vector>
#include <ctime>

#define IDLE_TIMEOUT 10 //timeout d'un client sans aucune 
#define KEEPALIVE_TIMEOUT 3
#define NO_TIMEOUT 0
#define TIMEOUT 1

typedef enum e_client_state
{
	CONNEXION_ACCEPTED,
	READING_REQUEST,
	WRITING_RESPONSE,
	READING_CGI,
	DONE_READING_CGI,
	DONE,
}	t_client_state;

class Config;
class Request;

class Client
{
	public:
		Client();
		Client(int fd, const Config *config, int ep_fd);
		Client(const Client& to_copy);
		Client& operator=(const Client& to_copy);
		~Client();

		int                 clientFd;
		int                 clientState;
		int                 epFd;

		std::vector<char>   writeBuff;
		size_t              bytesSent;
		size_t              buffSize;

		time_t              _lastActivity;

		Request             _request; //do copy
		int                 _requestCount;
		const Config* 		_serverConfig; // do copy
		bool                _keepAlive;

		//CGI DATA
		bool                isCGI;
		int                 pipeType;
		int                 ogFd; // fd du client original
		int                 forkPid;
		std::vector<char>   cgiResponseBuff;

		int                 timeout();
		void                updateActivity();
		time_t              getLastActivity() const;
		Request&            getRequest();
		const Config&       getConfig() const;
		int                 getFd() const;
		void                resetClient();
};

#endif