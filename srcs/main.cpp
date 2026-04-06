#include "../includes/server/Server.hpp"
#include "../includes/config/Parser.hpp"
#include "../includes/http/Response.hpp"
#include "../includes/Signals.hpp"
#include "../includes/Webserv.hpp"
#include "../includes/http/Response.hpp"


int main(int argc, char **argv)
{
	Webserv webserv;

	std::string configPath = "./default.conf";
	if (argc > 1)
		configPath = argv[1];

	try
	{
		Response::initMimeType();
		//PARSING
		Parser parser;
		std::vector<Server> servers;
		// std::cout << "Parsing config file: " << configPath << std::endl;
		std::vector<Config> configs = parser.parseFile(configPath);
		for (size_t i = 0; i < configs.size(); i++) {
			Server server(configs[i]);
			// configs[i].printConfig();
			servers.push_back(server);
		}
	
		//SIGNALS
		handle_signals();

		//INIT SERVER
		std::cout << std::endl << "Initializing servers..." << std::endl;
		try
		{
			webserv.setServers(servers);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what();
			std::cerr << strerror(errno) << std::endl;
			return 1;
		}
		
		

		//PARTIE POLL
		// std::cout << "Starting Epoll..." << std::endl;
		webserv.epollLoop();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what();
		std::cerr << strerror(errno) << std::endl;
		webserv.finalClean();
		return (1);
	}
	return (0);
}

// #include <iostream>
// #include <vector>
// #include <string>
// #include "../include/request.hpp"

// // Fonction utilitaire pour afficher l'état de la requête
// void printStatus(Request &req, const std::string &testName) {
//     std::cout << "===== TEST: " << testName << " =====" << std::endl;
//     std::cout << "Status:  " << (req.isFinished() ? "FINISHED" : "PROCESSING") << std::endl;
//     std::cout << "Method:  [" << req.getMethod() << "]" << std::endl;
//     std::cout << "Error:   " << req.getErrorCode() << std::endl;
//     if (req.isFinished()) {
//         std::cout << "Body:    " << req.getBody().size() << " bytes" << std::endl;
//         if (req.getErrorCode() == 0 && !req.getBody().empty())
//             std::cout << "Content: " << std::string(req.getBody().begin(), req.getBody().end()) << std::endl;
//     }
//     std::cout << "================================\n" << std::endl;
// }

// int main() {
//     // --- TEST 1: Requête simple et complète ---
//     {
//         Request req;
//         std::string raw = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
        
//         req.feeding(raw.c_str(), raw.size());
//         printStatus(req, "Simple GET");
//     }

//     // --- TEST 2: Requête Fragmentée (Simulation réseau lent) ---
//     {
//         Request req;
//         std::string part1 = "POST /api HTTP/1.1\r\nHost: localhost\r\nContent-Len";
//         std::string part2 = "gth: 5\r\n\r\nHe";
//         std::string part3 = "llo";

//         std::cout << "--- TEST: Fragmented POST ---" << std::endl;
        
//         req.feeding(part1.c_str(), part1.size());
//         std::cout << "After part 1: step is " << req.getStep() << " (Expected: START_LINE/HEADERS)" << std::endl;

//         req.feeding(part2.c_str(), part2.size());
//         req.feeding(part3.c_str(), part3.size());
        
//         printStatus(req, "Fragmented POST Final");
//     }

//     // --- TEST 3: Erreur de méthode (Minuscules - Doit faire 400) ---
//     {
//         Request req;
//         std::string bad = "get /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
        
//         // On simule le try/catch que tu auras dans ton serveur
//         try {
//             req.feeding(bad.c_str(), bad.size());
//         } catch (const std::exception& e) {
//             // Dans ton feeding, tu devrais normalement déjà catcher pour mettre _step = FINISHED
//             std::cout << "[DEBUG] Exception caught for lowercase method" << std::endl;
//         }
//         printStatus(req, "Lowercase Method (400)");
//     }

//     // --- TEST 4: Chunked Transfer Encoding ---
//     {
//         Request req;
//         std::string chunked = "POST /chunk HTTP/1.1\r\nHost: test\r\nTransfer-Encoding: chunked\r\n\r\n"
//                               "4\r\nWiki\r\n5\r\npedia\r\n0\r\n\r\n";
//         req.feeding(chunked.c_str(), chunked.size());
//         printStatus(req, "Chunked Transfer");
//     }

//     return 0;
// }

// #include <iostream>
// #include <vector>
// #include "../include/Config.hpp"
// #include "../include/request.hpp"
// #include "../include/response.hpp"
// #include "../include/Cgi.hpp"

// int main() {
//     std::cout << "--- DÉBUT DES TESTS WEBSERV ---" << std::endl;

//     // 1. SIMULATION DU PARSING (Config)
//     Config config;
//     config.setRoot("www");
    
//     Location cgiLoc;
//     cgiLoc.path = "/cgi-bin";
//     cgiLoc.root = "www/cgi-bin";
//     cgiLoc.cgi[".py"] = "/usr/bin/python3"; // On simule ce que le parser a lu
//     config.addLocation(cgiLoc);

//     std::cout << "[OK] Configuration initialisée (CGI .py configuré)" << std::endl;

//     // 2. SIMULATION D'UNE REQUÊTE CLIENT (Request)
//     Request req;
//     // On simule l'arrivée d'octets sur la socket
//     std::string rawRequest = 
//         "GET /cgi-bin/hello.py HTTP/1.1\r\n"
//         "Host: localhost\r\n"
//         "Content-Type: text/plain\r\n"
//         "Connection: keep-alive\r\n\r\n";
    
//     req.feeding(rawRequest.c_str(), rawRequest.size());
//     req.parseRequest();

//     std::cout << "[OK] Requête parsée : " << req.getMethod() << " " << req.getUri() << std::endl;

//     // 3. GÉNÉRATION DE LA RÉPONSE (Response + CGI)
//     Response res(config);
    
//     std::cout << "[INFO] Lancement de setResponseFinal..." << std::endl;
//     res.setResponseFinal(req);

//     // 4. AFFICHAGE DU RÉSULTAT FINAL
//     std::vector<char> result = res.getResponseFinal();
//     std::string finalStr(result.begin(), result.end());

//     std::cout << "\n--- RÉPONSE HTTP GÉNÉRÉE ---" << std::endl;
//     std::cout << finalStr << std::endl;
//     std::cout << "--- FIN DU TEST ---" << std::endl;

//     return 0;
// }
// #include <iostream>
// #include <map>
// #include <vector>
// #include <unistd.h>
// #include "../include/Client.hpp"
// #include "../include/Config.hpp"
// #include "../include/Webserv.hpp"

// // On simule la fonction de Webserv pour le test
// void simulated_checkIdleTimeout(std::map<int, Client>& clients) {
//     std::map<int, Client>::iterator it = clients.begin();
//     while (it != clients.end()) {
//         if (it->second.timeout() == TIMEOUT) {
//             std::cout << "\033[31m   [TIMEOUT] Expulsion du FD " << it->first << "\033[0m" << std::endl;
//             clients.erase(it++); 
//         } else {
//             ++it;
//         }
//     }
// }

// int main() {
//     Config mockConfig;
//     std::map<int, Client> clients;

//     std::cout << "\033[95m========================================\033[0m" << std::endl;
//     std::cout << "\033[95m       TEST DE FLUX : TIMEOUT LOOP      \033[0m" << std::endl;
//     std::cout << "\033[95m========================================\033[0m" << std::endl;

//     // 1. Création de clients avec des âges différents
//     std::cout << "[1] Initialisation des clients..." << std::endl;
    
//     clients.insert(std::make_pair(1, Client(1, &mockConfig))); // Client tout neuf
//     clients.insert(std::make_pair(2, Client(2, &mockConfig))); // Client qui va expirer
//     clients.insert(std::make_pair(3, Client(3, &mockConfig))); // Client qui va expirer vite

//     // On trafique leur _lastActivity pour simuler le passé
//     clients.at(2)._lastActivity -= 58; // Proche de la limite (60s)
//     clients.at(3)._lastActivity -= 59; // Très proche de la limite
    
//     std::cout << "   -> FD 1 : Actif (0s d'inactivité)" << std::endl;
//     std::cout << "   -> FD 2 : Inactif (58s d'inactivité)" << std::endl;
//     std::cout << "   -> FD 3 : Inactif (59s d'inactivité)" << std::endl;

//     // 2. Simulation de la boucle Epoll (On simule 5 tours de boucle)
//     std::cout << "\n[2] Démarrage de la boucle de surveillance..." << std::endl;
    
//     for (int sec = 0; sec < 5; ++sec) {
//         std::cout << "\n--- Tour de boucle : T + " << sec << "s ---" << std::endl;
        
//         // On simule le passage du temps réel sur TOUS les clients
//         // (Dans la vraie vie, c'est le temps système qui avance)
//         std::map<int, Client>::iterator it;
//         for (it = clients.begin(); it != clients.end(); ++it)
//             it->second._lastActivity -= 1; 

//         // Appel de ta fonction de nettoyage
//         simulated_checkIdleTimeout(clients);

//         if (clients.find(1) != clients.end())
//             std::cout << "   (FD 1 est toujours là...)" << std::endl;
        
//         usleep(100000); // Petite pause pour la lisibilité (0.1s)
//     }

//     std::cout << "\n\033[95m========================================\033[0m" << std::endl;
//     if (clients.size() == 1 && clients.count(1)) {
//         std::cout << "\033[32m[SUCCÈS] Le ménage a été fait au bon moment !\033[0m" << std::endl;
//     } else {
//         std::cout << "\033[31m[ERREUR] La logique de nettoyage a échoué.\033[0m" << std::endl;
//     }

//     return 0;
// }
// int main() {
//     // 1. SIMULER LA CONFIG (Le GPS)
//     Config mockConfig;
//     mockConfig.setPort(8080);
//     mockConfig.setRoot("./www");

//     // 2. SIMULER LE CLIENT (Le Conteneur)
//     // On crée un client manuellement comme si epoll venait d'accepter
//     Client testClient(42, &mockConfig); 

//     // 3. SIMULER L'ARRIVÉE DE DONNÉES (Le Réseau)
//     // On crée une requête HTTP brute sous forme de string
//     std::string rawRequest = "GET /index.html HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";

//     std::cout << "--- ETAPE 1 : RECEPTION ---" << std::endl;
//     // On injecte les données comme le ferait recv()
//     testClient.getRequest().feeding(rawRequest.c_str(), rawRequest.size());

//     // 4. TESTER TA LOGIQUE DE FUSION
//     if (testClient.getRequest().isFinished()) {
//         std::cout << "[OK] Requete detectee comme finie." << std::endl;

//         // On lance ta Response avec la config du client
//         Response res(testClient.getConfig());
//         res.setResponseFinal(testClient.getRequest());

//         // On recupere les resultats
//         testClient._keepAlive = !res.getCloseFd();
//         std::vector<char> fullRes = res.getResponseFinal();
//         testClient.writeBuff.assign(fullRes.begin(), fullRes.end());
        
//         std::cout << "--- ETAPE 2 : VERIFICATION ---" << std::endl;
//         std::cout << "Taille de la reponse generee : " << testClient.writeBuff.size() << " octets." << std::endl;
//         std::cout << "Keep-Alive memorise : " << (testClient._keepAlive ? "OUI" : "NON") << std::endl;
//     }
//     return 0;
// }
