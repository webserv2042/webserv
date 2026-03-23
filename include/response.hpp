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

#include "errors.hpp"

class	Config;
struct	Location;
class	Request;
class	Cgi;

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
		static std::map<int, std::string>			_statusMessage;
		static std::map<std::string, std::string>	_mimeType;
		bool										_isCgiExt;
		const Config								&_config;
		struct stat									_dataFile;
		const Location								*_structLocation;
		std::string									_pathExecCgi;
		bool										_closeFd;

	public:

		Response(const Config &configServer);
		~Response();

		// static std::map<int, std::string>			initMessageStatus();
		void										setResponseFinal(const Request &reqClient);
		const std::vector<char>						&getResponseFinal() const;
		void										setStatusCode(e_status_code code);
		void										addHeaders(const std::string &key, const std::string &value);
		void										setBodySize(const std::string &bodyHttp);

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
		void										searchFile();

		void    									parseCgi(const std::vector<char> &cgi);
        void    									parseHeadersCgi(const std::string &line);

};

#endif