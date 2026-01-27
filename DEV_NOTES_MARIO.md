# 📝 DEV NOTE : Initialisation du Serveur

**Sujet :** Création du socket d'écoute et mise en mode Non-Bloquant.

---

## 1. Vue d'ensemble
Si vous voulez comprendre ma partie ça devrait suffir.
Mon but est de créer un serveur minimal pour que Daniya puisse commencer à travailler.

### Configuration
J'utilise un max de constante pour avoir un code modulable et on peut faire un fichier
de configuration qui change les constantes pour que ce soit totalement modulable (c'est
qu'une idée du calme).

---

## 2. Comment ça marche ?
Voici les étapes pour avoir un cycle complet de communication entre deux programmes qui ne parlent
pas la même langue mais qui s'accordent sur un protocole.

1 - Creer le socket : socket().
2 - Rendre le port non zombie : setsockopt().
3 - Rendre non bloquant : fcntl().
4 - Associer une adresse/port : bind().
5 - Activer l'écoute : listen().
6 - Accepter les connexions : accept(). --> Daniya qui doit le faire

---

## 3. Comment ça se passe

On crée un socket et on le stocke dans un socketFD afin de pouvoir le manipuler par la suite.
On prépare la "carte d'identité" mais elle n'est pas lié au socket (la structure socketAdress).

Atuellement dans la mémoire de l'ordi il y a :

1) socketFD : Comme un telephone generique (avec juste un numero)
2) socketAdress : Une feuille ou il y a ecrit port 5090 et toutes les ip

On empeche le fd de se zoombifié et de devenir innacessible. (je vais le faire plus tard jattend de voir le bug causé par ça)
On rend le socket non bloquant (explications clairs partie 4).
On va utiliser bind pour coller la feuille au telephone.
Sans bind le socket existe mais ne sais pas qu'il doit repondre au numéro 5090.

Ensuite on utilise listen pour passer le socket en mode passif donc en ecoute.

Puis on accepte une communication entrante sur un socket passif (donc en mode ecoute si t'as suivi).

Ok à partir de là ça devrai être bon nous sommes connecte.

Mon code s'arrête ici mais globalement après :
Le client envoie un mssg --> donc le serveur doit le receptionner --> le serveur doit rep
au client --> le client reçoit le méssage.

Construction du strict minimum pour Daniya.

---

## 4. Focus Technique : Le mode Non-Bloquant (`fcntl`)
Pour respecter le sujet et permettre une connexion multiple il faut utiliser O_NONBLOCK.
La fonction int fcntl (int fdesc, int cmd, ...);
Elle permet de faire une operation sur le fd donné.
 fdesc -->fd que tu veux utiliser
 cmd --> quelle operation faire sur le fd.
 cmd :
	F_GETFL : retourne la configuration du socket
	F_SETFL : Mettre ou enlever des flags existants
	F_GETFD : Retourne le close-on-exec flag d'un fd
	F_SETFD : Mettre ou enlever le close-on-exec flag dun fd

Mon objectif avec cette fonction c'est de rendre le serveur non bloquant.
Cette fonction possède énormement de fonctionnalites.
Celle que je vais utiliser cest O_NONBLOCK.
Problème.. je peux pas lappliquer directement sinon jecrase potentiellement les autres options
et je risque de casser le socket.

DONC on va dabord regarder la configuration du socket actuel (comme en système embarqué) en utilisant le flag F_GETFL qui nous renvoie un entier (etat de la configuration).
Pour faire court ça fonctionne comme des interrupteurs (bit 1 ou 0) et nous on recupere ces infos.
On va pas refaire un cours sur les opérateur binaire mais globalement si tu veux ajouter une option tu utilises l'opérateur OU logique (|).
Maintenant que j'ai la nouvelle configuration je la renvoie au système via la commande F_SETFL.

--> Le code est validé et envoyé pour Daniya

---

## 5. La suite

Maintenant que nous avons un serveur minimal on peut commencer à regarder le fichier de configuration.
Comme son nom l'indique il va permettre de nous donner des informations utiles à la configuration de notre serveur.
Si vous avez bien suivi pour l'instant on configure manuellement le serveur avec :
PORT : 5090
socketFD :
sockaddr_in.sin_addr : INADDR_ANY toutes les ip
Ok maintenant je dois ajouter des informations presentes sur le fichier de configuration.
Il y a donc maintenant :
server_name
client_max_body_size
root
Et là tu commences à comprendre qu'il y a un problème car toutes ces choses font partis d'une requëte http MAIS on a aucune idee de comment fonctionne une requete http donc allons-y.

## 6. Requête hhtp : C'est quoi

HTTP est un protocole texte, c'est donc lisible par un humain.
Exemple :

	GET /index.html HTTP/1.1
	Host: www.google.com
	User-Agent: Chrome/90.0
	Accept: */*

Une requête est toujours decoupée en 3 parties par des sauts de ligne.

# A - Première ligne
Ligne la plus importante. Elle contient 3 infos séparées par des espaces.
Exemple : GET /index.html HTTP/1.1

1. La méthode (Qu'est ce que je veux faire ?)
GET
Il peut y en avoir des différentes

GET - recuperer
POST - publier
PUT - ajouter
DELETE - supprimer

2. L'URI - uniform ressource identifier (sur quel fichier je veux le faire ?)
/index.html

3. La version (quelle langue je parle ?)
HTTP/1.1

# B - Les Headers
C'est la métadonnée (infos sur la requête).
Format : Clé : Valeur suivi d'un retour à la ligne.
Host: www.google.com
Host: localhost:5090 c'est là que le serverName va servir
User-Agent: Chrome/90.0 --> Qui est le client (ici chrome)
Content-Length: 150 --> traduction : Après les headers il y a 150 octets de données.

# C - Le Body
Contenu utile et optionnel.
Dans un GET, il est vide car tu demandes
Dans un POST, il contient des données.

Le piège : \r\n, en programmation réseau le saut de ligne cest pas juste \n mais
bien \r\n (heritage des machines à ecrire).
La séparation entre Headers et Body cest \r\n\r\n.


## 5. La suite (bis)

Voilà maintenant qu'on a vu ce qu'est une requête dans sa construction on va pouvoir continuer.
Il y a donc maintenant :
	server_name (string)
Config : serveer_name localhost;
Pourquoi car si on heberges 3 sites sur la meme IP
client_max_body_size
root
JE VIENS DE ME RENDRE COMPTE QUE JE TRAVAILLE SUR LES REQUETES HTTP ALORS QUE JE DOIS FAIRE LE FICHIER DE CONFIGURATION HAHAHAH

Pour la peine je vais essayer de trouver un lien entre les deux pour pas que ce travail soit inutile.
Le serveur passe son temps à comparer ce qu'il a le droit de faire (config) et ce qu'il reçoit (HTTP).
Les 4 moments où ça se produit :

1 - Host vs server_name
Probleme : le socker reçoit une ip mais si cette IP héberge  10 sites differents lequel on doit lancer ?
--> Le serveur lit le header HTTP Host(ex : dicate.fr) ensuite il parcourt sa configuration pour trouver le bloc server qui possede le server_name dicate.fr
Sans ce lien le serveur est aveugle et ne sait pas pour qui est l'appel.

2 - Traduction d'adresse URI vs root
Problème : le client demande /index.html sauf que pour le serveur le fichier n'est pa à la racine donc il prend le chemin demandé par la requete HTTP /index.html et le colle devant le chemindéfini dans la config root /var/www/site

3 - La protection Content-Length vs client_max_body_size
Problème : Un client malveillant essaye d'envoyer un fichier de 10go via une requete POST si on l'accepte RAMs saturé serveur qui plante.
Avant de lire le contenu, le serveur regarde la taille Content-lenght (HTTP) et la compare avec la limite que j'ai autorisé via le fichier config si c'est superieur alors on rejette erreur 413.

4 - Gestion d'erreurs
Problème : la ressource demandee n'existe pas. Le code interne du serv genere un 404 not found (cf inception).
Au lieu d'envoyer juste un 404 le serveur verifie sa config et regarde sil a une consigne speciale pour le 404 si oui il remplace par le fichier indiqué /404_custom.html

Voilà c'est bien utile de connaitre le lien entre conf.serv et requete HTPP
