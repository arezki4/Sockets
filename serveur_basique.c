#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct User
{
	char nom[30];
	int age;	
}User;


void * function(void*arg)
{
	int socket = *(int*)arg;
	const char msg[] = "quel est votre nom et votre age ?";
	User user;
	send(socket, msg, strlen(msg)+1, 0);
	recv(socket, &user, sizeof(user), 0);
	printf("le client s'appelle %s et a %d ans\n", user.nom, user.age);
	close(socket);
	free(arg);
	pthread_exit(NULL);
}

/*int function()
{
  int nbr, i, f = 1;
 
  printf("Entrez un nombre pour calculer sa factorielle\n");
  scanf("%d", &nbr);
 
  for (i = 1; i <= nbr; i++)
    f = f * i;
 
  printf("Factorielle de %d = %d\n", nbr, f);
 
  return 0;
}*/



int main(void)
{
	//pthread_t clientThread;

	int socketServeur = socket(AF_INET/*ipv4*/, SOCK_STREAM/*tcp*/, 0);
	struct sockaddr_in addrServeur;//configurer l'adresse du serveur
	addrServeur.sin_addr.s_addr = inet_addr("127.0.0.1");//convertir l'addresse au type s_addr
	addrServeur.sin_family = AF_INET;//attribuer le type AF_INET a sin_family
	addrServeur.sin_port = htons(30000);//attribuer un port et le convertie en sin_port
//on appere les sockets au reseaux
	bind(socketServeur,(const struct sockaddr *)&addrServeur, sizeof(addrServeur));//connecter le serveur avec bind en convertissant sockaddr_in en addrserver grace a un pointeur
	printf("bind : %d\n", socketServeur);
//on ecoute
	listen(socketServeur, 5);//le serveur ecoute sur son socket et peut avoir jusqu'a 5 connexion en attente
	printf("listen\n");


	pthread_t threads[3];
	for(int i=0 ; i<3; i++)
	{
		//on accepte les connexions du client
		//recuperer les sockets des clients
		struct sockaddr_in addrClient;//on creer un sockaddr_in du client
		socklen_t csize = sizeof(addrClient);//on lui met la taille de type socklen_t
		int socketClient = accept(socketServeur, (struct sockaddr *)&addrClient, &csize);//creer un socket de type entier ou on va accepter au niveau du sockets serveur une adresse de type sockaddr du client et on lui met l'adresse de la taille
		printf("accept\n");

		printf("client: %d\n", socketClient);

		int *arg = malloc(sizeof(int));
		*arg = socketClient;
		pthread_create(&threads[i],NULL, function, arg);
	}

	for (int i = 0; i < 3; ++i)
	{
		pthread_join(threads[i], NULL); 
	}

	/*User user = {
		.nom = "Arezki",
		.age = 22
	};*/

	//send(socketClient, &user, sizeof(user), 0); //send va envoyer par l'intermediaire du socket client une donnée user en pointeur generique


	//close(socketClient);
	close(socketServeur);
	printf("close\n");

	return 0;

}