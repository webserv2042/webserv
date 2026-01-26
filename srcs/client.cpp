#include "../includes/server/Server.hpp"

#include <arpa/inet.h>

const int IPV4 = AF_INET;

#define CONNECTION_HOST "127.0.0.1"
#define LISTENING_PORT 5090
#define BUFFER_SIZE 1024

int main (void)
{

	int socketFD = socket(IPV4, SOCK_STREAM, 0);
	if (socketFD == -1)
	{
		std::cout << "(CLIENT) Echec d'initialisation du socket" << std::endl;
		exit(1);
	}

	// On veut convertir l'adresse ipv4 en binaire (inet_pton)

	struct sockaddr_in socketAddress;
	socketAddress.sin_family = IPV4;
	socketAddress.sin_port = LISTENING_PORT;

	int inetReturnCode = inet_pton(IPV4, CONNECTION_HOST, &socketAddress.sin_addr);

	if (inetReturnCode == -1)
	{
		std::cout << "(CLIENT) Adresse invalide ou non prise en charge" << std::endl;
		exit(1);
	}

	int socketAddressLenght = sizeof(socketAddress);
	int connectionStatus = connect(socketFD, (struct sockaddr*) &socketAddress, socketAddressLenght);

	if (connectionStatus == -1)
	{
		std::cout << "(CLIENT) Echec de la connexion au serveur" << std::endl;
		exit(1);
	}

	// Envoi d'un message

	const char	message[] = "Bonjour serveur, je suis le client";
	int			sentBytes = send(socketFD, message, strlen(message), 0);

	if (sentBytes == -1)
	{
		std::cout << "(CLIENT) Echec d'envoi du message du serveur" << std::endl;
		exit(1);
	}

	// Réception d'un message
	char	buffer[BUFFER_SIZE] = {0};
	int		receivedBytes = recv(socketFD, buffer, BUFFER_SIZE, 0);

	if (receivedBytes == -1)
	{
		std::cout << "(CLIENT) Echec de reception du message du serveur" << std::endl;
		exit(1);
	}
	printf("Serveur : %s\n", buffer);

	// Fermeture des sockets et libération des ressources
	close(socketFD);


	return (0);
}
