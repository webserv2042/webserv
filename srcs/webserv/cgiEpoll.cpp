#include "../../includes/Webserv.hpp"
#include "../../includes/server/Client.hpp"
#include "../../includes/Signals.hpp"
#include "../../includes/http/Response.hpp"
#include <cstdio>


/// @brief write to the CGI child process through a pipe
/// @param fd the fd of the pipe that communicates with the child
void    Webserv::CGIwriteToChild(int fd)
{
	std::cout << "CGI WRITING...." << std::endl;
	Request req = clients[fd].getRequest();

	if (req.getMethod() == "POST")
		write(fd, req.getBody().c_str(), req.getBody().size());

	closeClient(fd);
}

/// @brief read content given back by the CGI child
/// @param fd CGI read fd
void    Webserv::CGIreadFromChild(int fd)
{
	char    buffer[4096];
    int     ogClientFd = clients[fd].ogFd;
    
    // READ DATA FROM CHILD //
	// std::cout << "READING CGI...." << std::endl;
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
	// std::cout << "BTS READ: " << bytesRead << std::endl;
	// std::cout << "-->" << std::endl;

	// ONLY PART OF THE CGI HAS BEEN READ, WE STOP READING NOW AND RESUME WHEN EPOLL ALLOWS US //
    if (bytesRead > 0) {
		clients[fd].clientState = READING_CGI;
		// std::cout << "READ PARTIAL DATA FROM CLIENT" << std::endl;
        // Store data in the client's specific CGI buffer
        clients[fd].cgiResponseBuff.insert(clients[fd].cgiResponseBuff.end(), buffer, buffer + bytesRead);
        return; // Exit and wait for the next EPOLLIN event
    } 

	// READ ERROR //
	if (bytesRead == -1)
	{
		clients[fd].clientState = READING_CGI;
		// std::cout << "COULDNT READ BYTES FROM CLIENT YET, TRYING AGAIN" << std::endl;
		return; // No data yet, totally normal — epoll will notify us
	}

	// EVERYTHING HAS BEEN READ //
	else if ( bytesRead == 0)
	{
		clients[fd].clientState = DONE_READING_CGI;
		// std::cout << "READ THE WHOLE DATA FROM CLIENT, JOB DONE" << std::endl;

		// WAITPID //
		int status;
		waitpid(clients[fd].forkPid, &status, WNOHANG); //WNOHANG

		// AJOUT DEBUG
		std::string debug(clients[fd].cgiResponseBuff.begin(), clients[fd].cgiResponseBuff.end());
		std::cerr << "CGI RAW OUTPUT: [" << debug << "]" << std::endl;
		std::cerr << "CGI OUTPUT SIZE: " << clients[fd].cgiResponseBuff.size() << std::endl;

		// CLOSE CLIENT //
		std::vector<char> responseBuffer = clients[fd].cgiResponseBuff;
		closeClient(fd);

		if (responseBuffer.empty()) {
			throw std::runtime_error("500"); 
		}

		// FINISH THE RESPONSE //
		std::cout << "PREPARING FINAL CGI RESPONSE" << std::endl;
		CGIprepareResponse(ogClientFd, responseBuffer);
	}
}

/// @brief Finish the response from the CGI output
/// @param fd fd of the original client that made the request
/// @param cgiOutput the output given by CGI child
void    Webserv::CGIprepareResponse(int fd, std::vector<char> cgiOutput)
{
	const Config		&config = clients[fd].getConfig();
	Response			res(config);

	res.responseCgi(cgiOutput, clients[fd].getRequest());

	clients[fd]._keepAlive = !res.getCloseFd();

	std::vector<char>	responseToSend = res.getResponseFinal();

	clients[fd].writeBuff.assign(responseToSend.begin(), responseToSend.end());
	clients[fd].buffSize = clients[fd].writeBuff.size();
	clients[fd].bytesSent = 0;

	clients[fd].clientState = WRITING_RESPONSE;
	modifyEpollout(fd, ADD_EPOLLOUT);
}