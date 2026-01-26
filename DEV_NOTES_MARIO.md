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

---
