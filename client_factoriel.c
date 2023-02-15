#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct User
{
	int number;
}User;

int main(void)
{
	int socketClient = socket(AF_INET/*ipv4*/, SOCK_STREAM/*tcp*/, 0);
	struct sockaddr_in addrClient;//configurer l'adresse du client
	addrClient.sin_addr.s_addr = inet_addr("127.0.0.1");//convertir l'addresse au type s_addr
	addrClient.sin_family = AF_INET;//attribuer le type AF_INET a sin_family
	addrClient.sin_port = htons(30000);//attribuer un port et le convertie en sin_port

	connect(socketClient, (const struct sockaddr *)&addrClient, sizeof(addrClient));//on connecte le socket client à l'adresse de sockaddr_in qui correspond au serveur avec la taille 
	printf("connecé\n");

	User user;
	/*recv(socketClient, &user, sizeof(User),0); // on recois les données de la part du socket creer au part avant et on la place dans l'utilisateur en pointeur vue qu'on met une adresse memoire, de la taille de l'utilisateur
	printf("%s %d\n", user.nom, user.age);*/

	printf("Entrez un entier: ");
	scanf("%d", &user.number);
	send(socketClient, &user.number, user.number+1, 0);
	recv(socketClient, &user.number, sizeof(user.number), 0);
	printf("la factoriel de l'entier est = %d\n", user.number);

close(socketClient);
printf("close\n");
	close(socketClient);

	return 0;
}