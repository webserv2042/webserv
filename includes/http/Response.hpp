#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <vector>
#include <dirent.h>

#include "Utils.hpp"
#include "Errors.hpp"
#include "../server/Client.hpp"

#define PATH_ERROR_PAGES "www/errors_pages/"
#define IS_CGI			1

class	Config;
struct	Location;
class	Request;
class	CGI;

class Response
{
	private:
		e_status_code								_statusCode; 
		std::vector<char>							_responseFinal;
		std::vector<char>     						_body;
	
		std::string									_uriFullPath;
		std::string									_extension;
		std::string									_dateHttp;
		std::string									_locationUri;
		size_t										_contentLength;


		std::map<std::string, std::string>			_headers;
		std::vector<std::string>					_cookies;
		static std::map<e_status_code, std::string> _statusMessage;
		static std::map<std::string, std::string>	_mimeType;
	
		bool										_isCgiExt;
		std::string									_pathExecCgi;
	
		const Config								&_config;
		struct stat									_dataFile;
		const Location								*_structLocation;
		bool										_isAutoIndex;
		bool										_closeFd;

	public:

		Response(const Config &configServer);
		~Response();

		int											setResponseFinal(const Request &reqClient, int fd, std::map<int, Client> &clients);
		const std::vector<char>						&getResponseFinal() const;
		void										setStatusCode(e_status_code code);
		void										addHeaders(const std::string &key, const std::string &value);
		void										addCookie(const std::string &cookies);
		// void										setBodySize(const std::string &bodyHttp);
		void    									setHttpDate();
		void										setLocationUri(const std::string &path);
		std::string 								getUriFullPath() const;
		std::string 								getExtension() const;
		std::string 								getHeader(std::string key);
		std::string									getExt() const;
		std::string									getRootLocation() const;
		e_status_code								getStatusCode() const;
		bool										getCloseFd() const;
		
		static void									initMimeType();
		static void									isNotMimeFile();

		void										fullPathUri(const Request &req);
		void										setExtension();
		bool										isCgi();
		void										contentType();
		void										checkingUri(const Request &req);
		void										checkingPerm();
		void										searchFile(const Request &req);

		void										responseCgi(std::vector<char> cgiOutput, const Request &req);
		void    									parseCgi(const std::vector<char> &cgi);
        void    									parseHeadersCgi(const std::string &line);

		// method

		void    									methodProcess(const Request &req);
		std::string 								allowedMethods() const;
		void										doGet();
		void										doPost(const Request &req);
		void										doDelete();


        void    									createResponse();
        void    									setStartLine();
        void    									setHeaders(const Request &req);
        void   	 									doAutoIndex(const Request &req);
		static std::map<e_status_code, std::string>	initMessageStatus();
		void										generateErrorPage(e_status_code code);
		bool										checkFile(const std::string &path);
		std::string									readFile(const std::string &path);

		void										fail(e_status_code code);

};

#endif