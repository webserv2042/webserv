# 📝 DEV NOTE : Fonctionnement du serveur

**INTRODUCTION** Intro

---

Dans cette note, je vais essayer d'expliquer comment tourne le server, a partir du lancement de la boucle epoll (le server de base etant deja ete lance avec la partie socket)

Ce sujet est aborde dans les etapes suivantes: 
1) La boucle evenementielle epoll
2) Rendre le server non-bloquant
3) Gestion des timeouts

**PARTIE 1** La boucle evenementielle epoll

---

## 1 - Initialisation de l'instance epoll
Une instance epoll, c'est seulement un fd retourne par epoll_create:
ce fd represente une structure de donnees connectee au noyau de l'ordinateur

Cette structure nous permet de recuperer les informations suivantes:
1) la liste d'interet: les fd que l'on veut surveiller
2) la liste des fds prets: les fds prets a envoyer ou recevoir du contenu

## 2 - Ajout des fd des sockets (lies aux ports)
Une fois notre instance epoll creee, on lui donne nos socketFD, qui sont les portes d'entrees de notre serveur
on va ajouter ces socketFD a la liste d'interet mentionnee plus haut, avec la fonction epoll_ctl

pendant l'ajout de ce socketFD, il faut preciser quel type d'evenement nous interesse, ici c'est EPOLLIN
EPOLLIN signifie que notre fd est pret a etre lu

## 3 - Surveillance des evenements -> Debut de la boucle
Maintenant, on peut appeler la fonction epoll_wait qui va 'surveiller' les fds que l'on a mit sur notre liste d'interet juste avant

Des qu'un evenement se procure, epoll_wait se reveille et nous rend les informations de cet evenement dans un tableau struct epoll_event et il nous retourne le nombre d'event qui se sont produits

/!\ a cette etape il est important de verifier si le signal n'a pas ete interrompu

## 4 - Evenements detectes
Quand on recupere les evenements qui se sont produits sur nos fd, on retrouve 2 types de cas:
1) les evenements arrives sur le fd d'un socket
-> cela signifie qu'un nouveau client essaye de se connecter au server
    ->on accepte ce client:
    -un nouveau socket est cree pour lui (la fonction accept() retourne le fd de ce socket)
    -on ajoute ce socket a la liste d'interet, comme fait au debut pour nos ports
2) les evenements arrives sur le fd d'un client
-> un client nous envoi une requete prete a etre lue
    -on lit la requete
    -on la traite
    -on passe le client en mode reception de reponse
-> un client et pret a recevoir une reponse
    -on envoi la reponse
    -on repasse le client en mode envoi de requetes

## 5 - Repetition de la boucle
Une fois les requetes/reponses traitees/connections acceptees, la boucle reprend a epoll_wait et attend de nouveau


**PARTIE 3** Gestion des timeouts

---

## 1 - Que sont et a quoi servent les timeouts?
Les timeouts mettent une limite de temps sur les activites d'un serveur, ex:
un timeout de 60secondes est mit sur les activites, si un client se connecte au serveur, mais n'envoie pas de requetes pendant 60secondes, le client sera deconnecte.

Cela permet au serveur de ne pas laisser un client prendre de la place et des ressources inutilement et de bloquer le serveur. 
Cela protege aussi de certaines failles de securite, ex:
clients qui envoient des donnees tres lentement pour maintenir une connexion artificielle
-> le serveur est innonde de requetes, il ne peut plus repondre ou recevoir des requetes de reels clients

## 2 - Les differents types de timeout implementes
1) Timeout d'inactivité (Idle Timeout)
