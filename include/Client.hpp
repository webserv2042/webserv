#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Request.hpp"
#include "Config.hpp" // Pour le pointeur vers la config
#include <vector>
#include <ctime>

typedef enum e_client_state
{
    CONNEXION_ACCEPTED,
    READING_REQUEST,
    WRITING_RESPONSE,
    DONE
}	t_client_state;

class Config;
class Request;

class Client
{
    public:
        Client();
        Client(int fd, const Config* config);
        Client(const Client& to_copy);
        Client& operator=(const Client& to_copy);
        ~Client();

        // --- Données Réseau (Partie collègue) ---
        int                 clientFd;
        int                 clientState;     // Utilise l'enum ci-dessus
        std::vector<char>   writeBuff;       // La réponse finale à envoyer
        size_t              bytesSent;       // Combien d'octets on a déjà envoyé
        size_t              buffSize;        // Taille totale du message à envoyer
        time_t              _lastActivity;    // Pour le timeout

        // --- Données Métier (Ta partie) ---
        Request             _request;        // TON objet Request avec son _bytesData
        const Config* 		_serverConfig;    // Pointeur vers la config du serveur
        bool                _keepAlive;       // <--- LA VOILÀ !

        // --- Méthodes ---
        void                updateActivity();
        Request&            getRequest();
        const Config&       getConfig() const;
        int                 getFd() const;
        void                resetClient();
};

#endif