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