#include "../include/response.hpp"
#include "../include/request.hpp"
#include "../include/Config.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

void printTitle(const std::string& title) {
    std::cout << "\n\033[1;34m" << ">>> " << title << " <<<" << "\033[0m" << std::endl;
}

int main() {
    // Initialisation obligatoire des types MIME
    Response::initMimeType();
    Config mockConfig; // Utilise ta structure Config par défaut

    // --- PRÉPARATION DU TERRAIN ---
    // On crée le dossier errors_pages dans le répertoire d'exécution (srcs/)
    mkdir("errors_pages", 0777);
    
    // On crée une VRAIE page 404 personnalisée
    std::ofstream f("errors_pages/404.html");
    f << "<html><body><h1>DESIGN SENSHY : 404 CUSTOM</h1></body></html>";
    f.close();

    // ---------------------------------------------------------
    printTitle("TEST ERREUR 1 : LA VRAIE 404 (Lien Fichier)");
    {
        Request req;
        // On demande un fichier qui n'existe ABSOLUMENT PAS
        std::string raw = "GET /fantome_intergalactique.html HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
        req.feeding(raw.c_str(), raw.size());

        Response res(mockConfig);
        res.setResponseFinal(req); 

        const std::vector<char>& response = res.getResponseFinal();

        // On le convertit en string pour l'affichage (puisque c'est du texte ici)
        std::string fullResponse(response.begin(), response.end());

        std::cout << "--- CONTENU DE LA RÉPONSE ENVOYÉE ---" << std::endl;
        std::cout << fullResponse << std::endl;
        std::cout << "--------------------------------------" << std::endl;

        std::vector<char> rawRes = res.getResponseFinal();
        std::string s(rawRes.begin(), rawRes.end());

        if (s.find("HTTP/1.1 404") != std::string::npos && s.find("DESIGN SENSHY") != std::string::npos)
            std::cout << "\033[1;32m[SUCCESS]\033[0m Le serveur a detecte l'absence du fichier et a lu 404.html." << std::endl;
        else
            std::cout << "\033[1;31m[FAIL]\033[0m Verifie le contenu recu : \n" << s.substr(0, 100) << "..." << std::endl;
    }

    // ---------------------------------------------------------
    printTitle("TEST ERREUR 2 : MODE FALLBACK (Code sans fichier)");
    {
        Request req;
        // On simule une erreur 505 (HTTP Version Not Supported)
        // Comme on n'a pas cree de 505.html, il DOIT passer en fallback
        std::string raw = "GET /index.html HTTP/1.5\r\nHost: localhost\r\n\r\n";
        req.feeding(raw.c_str(), raw.size());

        Response res(mockConfig);
        res.setResponseFinal(req);

        const std::vector<char>& response = res.getResponseFinal();

        // On le convertit en string pour l'affichage (puisque c'est du texte ici)
        std::string fullResponse(response.begin(), response.end());

        std::cout << "--- CONTENU DE LA RÉPONSE ENVOYÉE ---" << std::endl;
        std::cout << fullResponse << std::endl;
        std::cout << "--------------------------------------" << std::endl;


        std::vector<char> rawRes = res.getResponseFinal();
        std::string s(rawRes.begin(), rawRes.end());

        if (s.find("Senshy Edition - Fallback Mode") != std::string::npos)
            std::cout << "\033[1;32m[SUCCESS]\033[0m Le mode Fallback a pris le relais pour la 505." << std::endl;
        else
            std::cout << "\033[1;31m[FAIL]\033[0m Le fallback n'a pas ete detecte." << std::endl;
    }

    // ---------------------------------------------------------
    printTitle("TEST ERREUR 3 : SECURITE (Tentative sur dossier)");
    {
        Response res(mockConfig);
        // On teste directement ta fonction checkFile sur le dossier errors_pages
        if (res.checkFile("errors_pages") == false)
            std::cout << "\033[1;32m[SUCCESS]\033[0m checkFile a refuse d'ouvrir le dossier comme un fichier." << std::endl;
        else
            std::cout << "\033[1;31m[FAIL]\033[0m checkFile a confondu un dossier et un fichier." << std::endl;
    }

    // --- NETTOYAGE ---
    unlink("errors_pages/404.html");
    rmdir("errors_pages");
    std::cout << "\nTests termines.\n" << std::endl;

    return 0;
}