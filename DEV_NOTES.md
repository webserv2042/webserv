Ces explications seront portés sur la partie de Mario.
Si vous voulez comprendre ma partie et comment elle fonctionne normalement ça devrai suffir.

J'utulise un max de constante pour avoir un code modulable et on peut faire un fichier
de configuration qui change les constantes pour que ce soit totalement modulable (c'est
qu'une idée du calme).

Dans un premier temps mon but va être de créer un serveur minimal pour que Daniya puisse commencer sa partie.
Ça veut dire quoi ?
C'est avoir un cycle complet de communication entre deux programmes qui ne parlent pas la même langue (serveur et nav web) mais qui saccordent sur un protocole.

Les étapes classiques côté serveur :
1 - Creer le socket : socket().
2 - Associer une adresse/port : bind().
3 - Activer l'écoute : listen().
4 - Accepter les connexions : accept(). --> Daniya qui doit le faire

~~~~~~~~~~
POURQUOI ?

Car si je lance le serveur et que j'arrive à "accept" le programme va s'arrêter et attendre un client donc Daniya ne va pas pouvoir lancer epoll/poll vu qu'elle est bloqué avant
~~~~~~~~~~

On crée un socket et on le stocke dans un socketFD afin de pouvoir le manipuler par la suite.
On prépare la "carte d'identité" mais elle n'est pas lié au socket.

Atuellement dans la mémoire de l'ordi il y a :

1) socketFD : Comme un telephone generique (avec juste un numero)
2) socketAdress : Une feuille ou il y a ecrit port 5090 et toutes les ip

On va utiliser bind pour coller la feuille au telephone.
Sans bind le socket existe mais ne sais pas qu'il doit repondre au numéro 5090.

Ensuite on utilise listen pour passer le socket en mode passif donc en ecoute.

Puis on accepte une communication entrante sur un socket passif (donc en mode ecoute si t'as suivi).

Ok à partir de là ça devrai être bon nous sommes connecte.

Le client envoie un mssg --> donc le serveur doit le receptionner --> le serveur doit rep
au client --> le client reçoit le méssage.

Construction du strict minimum pour Daniya.


