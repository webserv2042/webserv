Gestion des requêtes clients, réceptions, traitement et réponse. 

Condensé d'informations sur HTTP :

Partie théorique

Origine : crée par Til Berners-Lee et son équipe entre 1989 et 1991. A l'origine du web.

HTTP 0/9 = protocole à une ligne
HTTP 1/0 = 
    - le client ouvre une nouvelle connexion à chaque requête et la referme après, coûteux pour le processeur
    - l'en-tête Host n'est pas obligatoire car un serveur ne contenait généralement qu'un site
    - doit connaître la taille exacte du fichier pour l'envoyer (en-tête "content-length")
    - attente de la réponse avant d'envoyer une nouvelle requête 
HTTP 1/1 =
    - la connexion reste ouverte
    - en-tête Host obligatoire car un serveur peut héberger plusieurs sites (même ip et port)
    - possibilité d'envoyer une requête en plusieurs morceaux (en-tête "transfer-encoding:chunked)
    - peut envoyer plusieurs requêtes en même temps

Client : envoie des requêtes
Serveur : répond aux requêtes

Authentification HTTP :

- le serveur répond aux requêtes par un code 401 (unauthorized), le client s'authentifie.
- certificat TLS pour "assurer" la sécurité.
- authentification par procuration passe par un proxy, code 407 (proxy authentification required).
- si accès interdit, code 403 (forbidden).

TCP : protocole qui permet à deux hôtes de se connecter, distribue les paquets en respectant l'ordre dans lequel ils ont été envoyés.
Son rôle est d'assurer une livraison fiable.

Le navigateur (agent utilisateur: outil qui agit au service de l'utilisateur) est toujours celui qui initie la requête.

Le client communique avec le serveur : le serveur peut être un ordinateur, un load balancer ou une architecture logicielle 
qui génère totalement ou partiellement le doc à la demande.

Plusieurs serveurs peuvent être présents sur une même machine, et peuvent également partager la même adresse IP.

HOST : le champ d'en-tête HOST doit figurer sur chaque requête sinon erreur 400 (bad request),
le port TCP peut figurer mais pas ce n'est pas obligatoire.

Lorsque le client communique avec le serveur, il :
- ouvre une connexion TCP
- envoie un message http 
- lit la réponse du serveur
- ferme ou réutilise la connexion pour requêtes suivantes

Le CORS (Cross Origin Resource Sharing) : chaque site web est une bulle indépendante et privée,
seuls les en-têtes http peuvent permettre à d'autres sites web d'accéder à leurs ressources et de les utiliser.
(Pas utile pour le projet mais intéréssant, si un site x utilise les données d'un site y - exemple : la météo -, une en-tête doit figurer pour accorder 
l'autorisation).

Requête :
Une méthode (GET, POST etc)
Url de la ressource à extraire
Version du procotole http
En-tête optionnel pr transmettre informations supplémentaires pr le serveur
Ou un corps pour certaines méthodes comme POST
 
Réponse :
Version du protocole http suivi
Code de statut de la requête
Message de statut 
En-tête http
Date
HOST obligatoire
Potentiellemnt un corps avec la ressource demandée


-> Pour la requête client : si content-length = récupérer la requête en fonction de la taille, sinon chercher \r\n.
-> Pour la réponse à la requête, compter la taille de la requête réponse et l'envoyer dans content-length pour que le 
navigateur sache quand la page web se termine.

Pour requêtes statiques : content-length
Pour requêtes cgi (dynamiques) : chunked

Pour voir les requêtes d'un site quelconque : F12 -> network -> actualiser la page -> première ligne = requêtes (sinon curl)

Mise en cache : garder en copie une ressource demandée afin qu'elle soit renvoyée plus rapidement sans avoir à repasser
par le serveur. Ceci allège le serveur, améliore les performances en étant plus rapide pour transmettre les ressources au 
client. Utilisation limitée : les ressources ne restent pas indéfiniment inchangée donc important de bien configurer la mise 
en cache.


Partie technique

Manuels :

RFC 9110 (manuel HTPP)
cat 3 -> la cible d'une requête http est "une ressource". La ressource s'identifie par deux éléments, le host (quel site) ainsi 
que l'URI (quel fichier).
Le mapping sert à faire le lien entre la ressource demandée par la requête et la "traduction" sur le serveur, à savoir trouver la 
localisation de la ressource sur l'hôte. L'URI devient un chemin de fichier concret sur le serveur.

RFC 9112 (manuel HTTP/1.1)
cat 2.1 -> description de la syntaxe des requêtes
Syntaxiquement, la requête et la réponse sont identiques hormis la première ligne (request-line pour la requête et status code pour la 
réponse). C'est la première ligne qui distingue l'un et l'autre.

cat 2.2 -> parser le message http
Impératif de traiter la requête comme une séquence d'octet et ENSUITE comme un flux de caractère. Vérifier chaque caractère un à un puis ensuite 
former la string. Donc première partie du parsing en char, ensuite en string. Ceci pour protéger la string et l'empêcher de traiter une suite de 
caractères comme un caractère spécial. Code ascii < 127.
Bare CR = CR (\r) non suivi d'un "\n". Soit error 400 (bad request) soit remplacement par un espace.
Le parser doit ignorer au moins une ligne vide, pour anticiper les éventuels résidus des requêtes précédentes, jusqu'à trouver le début de la 
requête. Ajouter un timestamp pour surveiller le temps d'attente, si rien ne suit, fermer le socket manuellement.
Pour la réponse : AUCUN espace entre start-line et le premier header. Si espace, le destinataire doit soit rejeter le message, soit ignorer toutes les 
lignes précédées d'un espace.
Tout erreur dans le message doit renvoyer 400 (bad request).

à partir de cat 3-> syntaxe complète


//*******************************************************//

Notes sur le code :

Recv est l'équivalent de read avec l'argument du flag en plus. Recv lit dans le buffer le nombre d'octets 
passé en paramètres (sizeof(buffer)) et les stock dans buffer.
Rép :
- si > 0 -> bytes trouvés, on ajoute à request
- si == 0 -> fin de la requête, on ferme le client
- si < 0 -> erreur ou aucun byte, on gère l'erreur et ferme le client

Si Transfer-Encoding chunked est présent dans les headers, on ignore le Content-Length

Trailers : en présence de chunked, des headers peuvent être placés après le body (voir cat 7.1.2 de la RFC 9112)

Parsing :

Request-line = Method SP target SP version

std::distance(première occuration, deuximère occuration) -> pour avoir la taille d'un extrait 

Nginx ajoute simplement l'uri au root, si l'uri doit être modifiée il faut utiliser un alias. -> installer nginx pour faire le 
test de son propre comportement basique. 

Sources :

https://developer.mozilla.org/fr/docs/Web/HTTP/Guide
https://beej.us/guide/bgnet/


load balancing (pas essentiel pour le projet, juste un truc que j'ai bcp aimé apprendre) :
                https://www.ovhcloud.com/fr/learn/what-is-load-balancing/
                https://datatracker.ietf.org/doc/html/rfc1794
                https://datatracker.ietf.org/doc/html/rfc2391
                http://pdfs.loadbalancer.org/v8/loadbalanceradministrationv8.2.pdf

en-tête http :  https://www.iana.org/assignments/http-fields/http-fields.xhtml
code réponse :  
                https://www.iana.org/assignments/http-status-codes/http-status-codes.xhtml

dirent.h :      http://sdz.tdct.org/sdz/arcourir-les-dossiers-avec-dirent-h.html#:~:text=h.%20%3B),%2C%20fichiers...).
                https://www.emagister.fr/formation_parcourir_dossiers_dirent_h-ec2696453.htm

HTTP sémantique : https://datatracker.ietf.org/doc/html/rfc9110

errno :         https://docs.python.org/fr/3.8/library/errno.html

NGINX :         https://nginx.org/en/docs/http/ngx_http_core_module.html#root
