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

#include "utils.hpp"
#include "errors.hpp"

#define PATH_ERROR_PAGES "errors_pages/"

class	Config;
struct	Location;
class	Request;
class	CGI;

class Response
{
	private:
		e_status_code								_statusCode; // -> tu set l'état du status pour que je sache si tout est ok ou si je dois écrire une erreur dans ma réponse
		std::vector<char>							_responseFinal; // -> ça c'est pour moi la version finale
		std::vector<char>     						_body; // -> tu m'injectes le corps du script
		std::string									_uriFullPath;
		std::string									_extension;
		std::string									_dateHttp;
		std::string									_locationUri;
		std::map<std::string, std::string>			_headers; // t'ajoutes les headers nécéssaires (comme content-length notamment je te laisse te renseiger)
		static std::map<e_status_code, std::string>	_statusMessage;
		static std::map<std::string, std::string>	_mimeType;
		bool										_isCgiExt;
		const Config								&_config;
		struct stat									_dataFile;
		const Location								*_structLocation;
		std::string									_pathExecCgi;
		bool										_isAutoIndex;
		bool										_closeFd;

	public:

		Response(const Config &configServer);
		~Response();

		void										setResponseFinal(const Request &reqClient);
		const std::vector<char>						&getResponseFinal() const;
		void										setStatusCode(e_status_code code);
		void										addHeaders(const std::string &key, const std::string &value);
		void										setBodySize(const std::string &bodyHttp);
		void    									setHttpDate();
		void										setLocationUri(const std::string &path);

		std::string 								getUriFullPath() const;
		std::string 								getExtension() const;
		std::string 								getHeader(std::string key);
		std::string									getExt() const;
		e_status_code								getStatusCode() const;
		
		static void									initMimeType();
		static void									isNotMimeFile();

		void										fullPathUri(const Request &req);
		void										setExtension();
		bool										isCgi();
		void										contentType();
		void										checkingUri(const Request &req);
		void										checkingPerm();
		void										searchFile(const Request &req);

		// void										trim(std::string &line);
		void    									parseCgi(const std::vector<char> &cgi);
        void    									parseHeadersCgi(const std::string &line);

		// method

		void										doGet();
		void										doPost(const Request &req);
		void										doDelete();


        void    									createResponse();
        void    									setStartLine();
        void    									setHeaders(const Request &req);
        void    									methodProcess(const Request &req);
        void   	 									doAutoIndex(const Request &req);
		static std::map<e_status_code, std::string>	initMessageStatus();
		void										generateErrorPage(e_status_code code);
		bool										checkFile(const std::string &path);
		std::string									readFile(const std::string &path);

		void										fail(e_status_code code);




};

#endif