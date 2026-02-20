# 📝 DEV NOTE : Fonctionnement du serveur


## INTRODUCTION

Dans cette note, je vais expliquer(avec mes mots) comment tourne le serveur, à partir du lancement de la boucle epoll (le server de base étant déjà été lancé avec la partie socket -> voir DEV_NOTES_MARIO.md)

Ce sujet est abordé dans les étapes suivantes: 
1) La boucle évènementielle epoll
2) Rendre le serveur non-bloquant
3) Gestion des timeouts

## PARTIE 1 - La boucle évènementielle epoll

### 1 - Initialisation de l'instance epoll
Une instance epoll, c'est seulement un fd retourné par `epoll_create`:
ce fd représente une structure de données connectée au noyau de l'ordinateur

Cette structure nous permet de récupèrer les informations suivantes:
1) la liste d'intérêt: les fd que l'on veut surveiller
2) la liste des fds prêts: les fds prêts à envoyer ou recevoir du contenu

### 2 - Ajout des fd des sockets (liés aux ports)
Une fois notre instance epoll créee, on lui donne nos socketFD, qui sont les portes d'entrées de notre serveur
On va ajouter ces socketFD à la liste d'intérêt mentionnée plus haut, avec la fonction `epoll_ctl`

Pendant l'ajout de ce socketFD, il faut préciser quel type d'évènement nous intéresse, ici c'est EPOLLIN
EPOLLIN signifie que notre fd est prêt a être lu

### 3 - Surveillance des évènements -> Début de la boucle
Maintenant, on peut appeler la fonction `epoll_wait` qui va 'surveiller' les fds que l'on a mit sur notre liste d'intérêt juste avant

Dès qu'un évènement se produit, epoll_wait se réveille et nous rend les informations de cet évènement dans un tableau struct epoll_event et il nous retourne le nombre d'event qui se sont produits

/!\ À cette etape il est important de vérifier si le signal n'a pas été interrompu

### 4 - Evenements detectés
Quand on récupère les évènements qui se sont produits sur nos fd, on retrouve 2 types de cas:
1) Les évènements arrivés sur le fd d'un socket
-> cela signifie qu'un nouveau client essaye de se connecter au serveur
    ->on accepte ce client:
    -un nouveau socket est crée pour lui (la fonction `accept()` retourne le fd de ce socket)
    -on ajoute ce socket a la liste d'intérêt, comme fait au debut pour nos ports
2) Les évènements arrivés sur le fd d'un client
-> un client nous envoi une requête prête a être lue
    -on lit la requête
    -on la traite
    -on passe le client en mode réception de réponse
-> un client et prêt a recevoir une réponse
    -on envoi la réponse
    -on repasse le client en mode envoi de requêtes

### 5 - Répétition de la boucle
Une fois les requêtes/réponses traitées/connections acceptées, la boucle reprend a epoll_wait et attend de nouveau


## PARTIE 2 - Rendre le serveur non-bloquant

### 1 - Qu'est-ce qu'un serveur non-bloquant?
C'est un serveur qui ne bloque pas les connexions/activités des clients par celles des autres, ex:
Un client est surveillé en attente de réponse, si rien n'est envoyé immédiatement la fonction d'envoi s'arrête est permet au programme d'écouter/gérer les autres activités du serveur comme une nouvelle connexion client

### 2 - Comment le serveur est-il non-bloquant? 
1) Les sockets
Lorsque les sockets de nos ports ou de nos clients sont crées, on les rend non-bloquants grâce à la fonction `fcntl()`
-> tant que rien ne se passe sur le fd de ces sockets, il ne bloqueront pas le programme

2) Lecture et écriture
Les fonctions de lecture(recve, read, ...) et d'écriture(send, ...) sont aussi mises en non-bloquant, voilà comment ça fonctionne:
    -on essaye de lire/écrire les données -> si c'est une petite quantité de données, tout est lu/écrit d'un coup
    -si les données sont plus conséquentes, le buffer de lecture/écriture ne peut pas tout stocker alors une partie seulement est lue/écrite
    -c'est ici que vient la différence avec un serveur bloquant: au lieu d'attendre le reste des données à écrire/lire, la fonction va s'arrêter, le programme continue sa boucle afin de reçevoir les autres évènements, puis seulement après va reprendre la lecture/écriture ->le serveur n'est pas bloqué !


## PARTIE 3 - Gestion des timeouts

### 1 - Que sont et a quoi servent les timeouts?
Les timeouts mettent une limite de temps sur les activités d'un serveur, ex:
un timeout de 60secondes est mit sur les activités, si un client se connecte au serveur, mais n'envoie pas de requêtes pendant 60 secondes, le client sera déconnecté.

Cela permet au serveur de ne pas laisser un client prendre de la place et des ressources inutilement et de bloquer le serveur. 
Cela protège aussi de certaines failles de securite, ex:
clients qui envoient des donnees très lentement pour maintenir une connexion artificielle
-> le serveur est innonde de requêtes, il ne peut plus répondre ou reçevoir des requêtes de réels clients

### 2 - Les differents types de timeout
1) Timeout d'inactivité (Idle Timeout)
Ce timeout permet tout simplement de mettre une limite sur le temps d'inactivité d'un client, au bout de laquelle il sera déconnecté

2) Timeout de connexion totale
Limite le temps total de connexion d'un client, même si de l'activité est reçue de sa part

3) Timeout de lecture
Limite le temps que prend la lecture d'une requête client : si la requête entière n'a pas été reçue au bout de X temps -> client déconnecté

4) Timeout d'écriture
Limite le temps que prend l'envoi total de la réponse vers le client