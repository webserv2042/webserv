#include "../../includes/http/Cgi.hpp"

CGI::CGI() : _clientFd(-1) {
}

CGI::CGI(const std::string &pathScript, const std::string &pathExec)
	: _pathScript(pathScript), _pathExec(pathExec), _clientFd(-1) {
}

CGI::~CGI() {
}

void CGI::setClientFd(int fd) {
	_clientFd = fd;
}

char** vectorToChar(std::vector<std::string> env) {
	char **result = new char*[env.size() + 1];
	for (size_t i = 0; i < env.size(); i++) {
		result[i] = strdup(env[i].c_str());
	}
	result[env.size()] = NULL;
	return (result);
}

void CGI::buildEnvp(const Request &req, const Response &rep) {
	std::ostringstream oss;
	oss << req.getContentLength();

	_envp.push_back("REQUEST_METHOD=" + req.getMethod());
	_envp.push_back("CONTENT_LENGTH=" + oss.str());
	_envp.push_back("CONTENT_TYPE=" + req.getHeader("Content-Type"));
	_envp.push_back("QUERY_STRING=" + req.getQueryString());
	_envp.push_back("PATH_TRANSLATED=" + rep.getUriFullPath());
	_envp.push_back("SCRIPT_NAME=" + req.getUri());
	_envp.push_back("SERVER_PROTOCOL=" + req.getHttpVersion());
	_envp.push_back("REDIRECT_STATUS=200");
	_envp.push_back("DOCUMENT_ROOT=" + rep.getRootLocation());
	_envp.push_back("SCRIPT_FILENAME=" + rep.getUriFullPath());
	_envp.push_back("HTTP_HOST=" + req.getHeader("Host"));

	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	std::string remoteAddr = "";
	if (_clientFd != -1 && getpeername(_clientFd, (struct sockaddr *)&addr, &len) == 0)
		remoteAddr = std::string(inet_ntoa(addr.sin_addr));
	_envp.push_back("REMOTE_ADDR=" + remoteAddr);
	_envp.push_back("GATEWAY_INTERFACE=CGI/1.1");
}

std::vector<char> CGI::getOutput() const {
	return _output;
}

void	CGI::execCgi(const Request &req) {
	int		pipeIn[2];
	int 	pipeOut[2];
	pid_t	forkPid;

	if (pipe(pipeIn) == -1) {
		throw std::runtime_error("(CGI) : Error while creating pipe");
	}

	if (pipe(pipeOut) == -1) {
		close(pipeIn[0]);
		close(pipeIn[1]);
		throw std::runtime_error("(CGI) : Error while creating pipe");
	}
	forkPid = fork();
	if (forkPid == -1) {
		close(pipeIn[0]);
		close(pipeIn[1]);
		close(pipeOut[0]);
		close(pipeOut[1]);
		throw std::runtime_error("(CGI) : Error while forking");
	}
	if (forkPid == 0) {
		close(pipeIn[1]);
		close(pipeOut[0]);
		if (dup2(pipeIn[0], STDIN_FILENO) == -1) {
			close(pipeIn[0]);
			close(pipeOut[1]);
			exit(1);
		}
		if (dup2(pipeOut[1], STDOUT_FILENO) == -1) {
			close(pipeIn[0]);
			close(pipeOut[1]);
			exit(1);
		}
		close(pipeIn[0]);
		close(pipeOut[1]);
		if (chdir(this->_pathScript.substr(0, _pathScript.rfind('/')).c_str()) == -1) {
			exit(1);
		}
		char **envp = vectorToChar(this->_envp);
		char *argv[] = {(char*)_pathExec.c_str(), (char*)_pathScript.c_str(), NULL};
		execve(this->_pathExec.c_str(), argv, envp);
		for (size_t i = 0; i < _envp.size(); i++)
     		free(envp[i]);
		delete[] envp;
		exit(1);
	}
	else {
		close(pipeIn[0]);
		close(pipeOut[1]);
		if (req.getMethod() == "POST") {
			write(pipeIn[1], req.getBody().c_str(), req.getBody().size());
		}
		close(pipeIn[1]);
		char 	buffer[4096];
 		ssize_t	bytesRead;
		while ((bytesRead = read(pipeOut[0], buffer, sizeof(buffer))) > 0) {
			_output.insert(_output.end(), buffer, buffer + bytesRead);
		}
		int status;
		waitpid(forkPid, &status, 0);
		// AJOUT DEBUG
		std::string debug(_output.begin(), _output.end());
		std::cerr << "CGI RAW OUTPUT: [" << debug << "]" << std::endl;
		std::cerr << "CGI OUTPUT SIZE: " << _output.size() << std::endl;

		close(pipeOut[0]);

		// if (_output.empty()) {
        //     throw std::runtime_error("500"); 
        // }
	}
}

std::vector<char>	CGI::cgi(const Request &req, Response &rep) {
	buildEnvp(req, rep);
	execCgi(req);
	return (_output);
}
