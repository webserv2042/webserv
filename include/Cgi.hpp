#ifndef CGI_HPP
# define CGI_HPP

#include "request.hpp"
#include "response.hpp"
#include <vector>
#include <cstring>
#include <sys/wait.h>	
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class CGI {
	private:
		std::vector<char>			_output;
		std::vector<std::string>	_envp;
		std::string					_pathScript;
		std::string					_pathExec;
		int							_clientFd;

	public:
		CGI();
		CGI(const std::string &pathScript, const std::string &pathExec);
		~CGI();
		void						setClientFd(int fd);
		void						buildEnvp(const Request &req, const Response &rep);
		void						execCgi(const Request &req);
		std::vector<char>			getOutput() const;
		std::vector<char>			cgi(const Request &req, Response &rep);
};

#endif
