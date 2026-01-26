#include "../includes/server/Server.hpp"

int main (void)
{
	/*  1 - Création du socket
		Crée une extremite de communication et retourne un file descriptor qui refere
		à cette extremite
	*/
	int socketFD = socket(IPV4, SOCK_STREAM, 0);
	if (socketFD == -1)
	{
		std::cout << "(SERVER) Echec d'initialisation du socket" << std::endl;
		exit(1);
	}

	/*	2 - Associer un port (liaison)
		Comme la carte d'idendité réseau IPv4 d'un socket.
		Sans elle le socket peut exister mais ne sait pas ni où ecouter, ni où se connecter
		structure avec differentes informations :
		sin_family --> AF_INET IPV4
		sin_port --> port de communication quon va utiliser
		struct in_addr sin_addr --> adresse qui nous concerne (nous on veut communiquer avec
		n'importe quel client mais on peut filtrer).
	*/
	struct sockaddr_in socketAddress;
	socketAddress.sin_family = IPV4;
	socketAddress.sin_port = LISTENING_PORT;
	socketAddress.sin_addr.s_addr = INADDR_ANY;

	int socketAddressLenght = sizeof(socketAddress);
	int bindReturnCode = bind(socketFD, (struct sockaddr*) &socketAddress, socketAddressLenght);

	if (bindReturnCode == -1)
	{
		std::cout << "(SERVER) Echec de liaison pour le socket" << std::endl;
		exit(1);
	}
	/* Maintenant que tout est lié on va pouvoir mettre le serveur sur ecoute
		attendre des connections */

	/*	3 - Activer l'ecoute (attente de nvl connexions)
		Ici, listen va transformer un socket actif en socket passif pret a accepeter les connexions
		socketfd : le descripteur du socket que jai cree et lie avec bind
		backlog : nombre max de co en attente qu le systeme oeut garder en file avant de refuser les nvl*/

	if (listen(socketFD, PENDING_QUEUE_MAXLENGTH) == -1)
	{
		std::cout << "(SERVER) Echec de demmarage de l'écoute" << std::endl;
		exit(1);
	}

	puts("En attente de nouvelles connexions ...");

	/*	4 - Accepter les connexions
		Sert a accepter une connexion entrante sur un socket passif (celui qu'on
		a listen).
		socket fd : le socket passif sur lequel le serv ecoute
		addr : pointeur vers une struct qui et remplie avec ladresse du clien (IP + port)
		addrlen : taille de la struct addr.
	*/

	int connectedSocketFD = accept(socketFD, (struct sockaddr*) &socketAddress,
									(socklen_t *) &socketAddressLenght);

	if (connectedSocketFD == -1)
	{
		std::cout << "(SERVER) Echec d'etablissement de la connexion" << std::endl;
		exit(1);
	}

	// Réception d'un message
	char	buffer[BUFFER_SIZE] = {0};
	int		receivedBytes = recv(connectedSocketFD, buffer, BUFFER_SIZE, 0);

	if (receivedBytes == -1)
	{
		std::cout << "(SERVER) Echec de reception du message du client" << std::endl;
		exit(1);
	}

	printf("Client : %s\n", buffer);

	// Envoi d'un message

	const char	message[] = "Bonjour client, je suis le serveur.";
	int			sentBytes = send(connectedSocketFD, message, strlen(message), 0);

	if (sentBytes == -1)
	{
		std::cout << "(SERVER) Echec d'envoi du message au client" << std::endl;
		exit(1);
	}

	// Fermeture des sockets et libération des ressources
	close(connectedSocketFD);
	close(socketFD);


	return (0);
}
