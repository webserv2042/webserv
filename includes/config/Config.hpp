#ifndef CONFIG_HPP

# define CONFIG_HPP

#include <cstring>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <utility>

const int IPV4 = AF_INET;

#define PENDING_QUEUE_MAXLENGTH 1

struct Location {
	std::string						path; // identifiant --> "/admin" "/images"
	// regles qui peuvent ecraser celles du serv
	std::string						root; // nouveau dossier racine pour cette route
	std::vector<std::string>		index;
	bool							autoIndex;
	std::vector<std::string>		allowedMethods;
	std::pair<int, std::string>		returnRedirect;
	std::string						uploadPath;
	// std::map<std::string, std::string>	cgi;
	Location() : autoIndex(false) {}
};

class Config {
	private:
		int							_port;	  // Stocker le port
		size_t						_clientMaxBodySize;
		std::vector<std::string>	_serverName; // peut etre en std::vector car plusieurs name
		std::string					_host;
		std::string					_root;
		std::string					_index; // ou bien std::vector<string> plus tard
		std::string					_uploadPath;
		std::map<int, std::string>	_errorPage;
		std::vector<std::string>	_allowedMethods;
		std::pair<int, std::string>	_return;
		bool						_autoIndex;
		std::vector<Location>		_locations;

	public:
		Config();

		// ----- getters ----
		// types simples (copie)
		int									getPort() const;
		size_t								getClientMaxBodySize() const;
		bool								getAutoIndex() const;

		// types complexes (référence constante pour la rapidité)
		const std::vector<std::string>&		getServerNames() const; // getter global
		const std::string&					getServerName(size_t index = 0) const; // renvoyer le vecteur
		const std::string&					getHost() const;
		const std::string&					getRoot() const;
		const std::string&					getIndex() const;
		const std::string&					getUploadPath() const;
		const std::map<int, std::string>&	getErrorPage() const;
		const std::vector<std::string>&		getAllowedMethods() const;
		const std::vector<Location>&		getLocations() const;

		// ----- setters -----
		// par le parser
		void								setPort(int value);
		void								setClientMaxBodySize(size_t value);
		void								setHost(const std::string& value);
		void								setRoot(const std::string& value);
		void								setIndex(const std::string& value);
		void								setUploadPath(const std::string& value);
		void								setAutoIndex(bool value);

		// ajout petit à petit
		void								addServerName(const std::string& name);
		void 								addErrorPage(int code, const std::string& path);
		void								addAllowedMethod(const std::string& method);
		void								addLocation(const Location& location);

		// test afficher
		void 								printConfig() const;


};

#endif
/*
# Fichier: webserv.conf

server {
    listen 8080;
    host 127.0.0.1;
    server_name localhost;

    # Taille max du corps de la requête client
    client_max_body_size 10M;

    # Page d'erreur par défaut
    error_page 404 /error_pages/404.html;

    # Dossier racine du site
    root /var/www/site_principal;
    index index.html;

    # Routes spécifiques
    location / {
        allow_methods GET POST;
        autoindex on;
    }

    location /images {
        root /var/www/site_principal/images;
        allow_methods GET;
    }
}

server {
    listen 8081;
    host 127.0.0.1;
    server_name test_server;

    root /var/www/site_test;
    index home.html;

    location / {
        allow_methods GET DELETE;
    }
}
*/
