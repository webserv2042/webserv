#include "../include/response.hpp"
#include "../include/request.hpp"
#include <iostream>

void test_complet(const std::string& label, const std::string& raw_http_request) {
    std::cout << "--- TEST : " << label << " ---" << std::endl;

    // 1. On utilise ton système de réception
    Request req;
    req.feeding(raw_http_request.c_str(), raw_http_request.size());
    
    // 2. On utilise ta fonction de parsing (dans parseRequest.cpp)
    req.parseRequest();

    Response res;
    try {
        // 3. On utilise ta fonction de logique (dans treatmentUri.cpp)
        res.checkingUri(req);
        
        std::cout << "SUCCESS" << std::endl;
        std::cout << "Status Code : " << res.getStatusCode() << std::endl;
        std::cout << "Path final  : " << res.getUriFullPath() << std::endl;
    }
    catch (const std::exception& e) {
        // Si checkingUri() a fait un throw suite à un NOT_FOUND ou FORBIDDEN
        std::cout << "ERREUR DETECTEE" << std::endl;
        std::cout << "Status Code : " << res.getStatusCode() << std::endl;
    }
    std::cout << "-------------------------------------------\n" << std::endl;
}

int main() {
    // On n'oublie pas ton initialisation static
    Response::initMimeType();

    // Test 1 : Un fichier qui existe
    test_complet("FICHIER EXISTANT", "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n");

    // Test 2 : Un dossier (va chercher index.html via ta fonction searchFile)
    // Assure-toi que le dossier www/test_dir/ existe
    test_complet("DOSSIER", "GET /test_dir/ HTTP/1.1\r\nHost: localhost\r\n\r\n");

    // Test 3 : Un fichier qui n'existe pas (doit trigger ton throw NOT_FOUND)
    test_complet("404", "GET /existe_pas.html HTTP/1.1\r\nHost: localhost\r\n\r\n");

    return 0;
}