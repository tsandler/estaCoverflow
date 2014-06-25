/*
 * kernelSocket.c
 *
 *  Created on: 18/06/2014
 *      Author: utnso
 */

#include "kernelSocket.h"
#include <commons/string.h>

#define BROKEN "BROKEN"
#define MAXBUFFSIZE 1024
#define GREAT "GREAT"


extern t_queue *NEW;
extern sem_t mutexNEW;
extern sem_t hayAlgo;
extern t_log*logs;

int openSocketClient(int port, char* ip) {

	int unSocket;
	struct sockaddr_in socketInfo;
	if ((unSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error al crear socket");
		return EXIT_FAILURE;
	}

	socketInfo.sin_family = AF_INET;
	socketInfo.sin_addr.s_addr = inet_addr(ip);
	socketInfo.sin_port = htons(port);

// Conectar el socket con la direccion 'socketInfo'.
	if (connect(unSocket, (struct sockaddr*) &socketInfo, sizeof(socketInfo))
			!= 0) {
		perror("Error al conectar socket");
		return EXIT_FAILURE;
	}

	printf("Conectado!\n");
	return unSocket;
}
char* sendMessage(int sockfd, char *msg) {

	int len;
	len = strlen(msg);

	if (send(sockfd, msg, len, 0) == -1) {
		return BROKEN;
	}
	return GREAT;

}

char* recieveMessage(int sock) {
	//int recibirDatos(int socket, tam, void** datos, t_log* logs);

	char *buffer = (char *) malloc(MAXBUFFSIZE);

	if (recv(sock, buffer, MAXBUFFSIZE, 0) <= 0) {
		return BROKEN;
	}

	return buffer;
}
int openSocketServerPLP(int PORT) {

	fd_set master;
	fd_set read_fds;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	int listener;
	int newfd;
	char buf[MAXBUFFSIZE];
	//int nbytes;
	/* para setsockopt()  */
	int yes = 1;
	t_length* tam = malloc(sizeof(t_length));

	FD_ZERO(&master); //que se yo q hace pero tiene  estar
	FD_ZERO(&read_fds);

	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Server-socketPLP() error lol!");
		/*just exit lol!*/
		exit(1);
	}

	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
			== -1) {
		perror("errpr em setsockoptPLP!");
		exit(1);
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(PORT);   //config del server
	memset(&(serveraddr.sin_zero), '\0', 8);

	if (bind(listener, (struct sockaddr *) &serveraddr, sizeof(serveraddr))
			== -1) {
		perror("Bind salio mal!");
		exit(1);
	}

	if (listen(listener, 10) == -1) // LA MAXIMA CANTDAD DE CONEXIONES ES 10
			{
		perror("error en listenPLP()!");
		exit(1);
	}

	FD_SET(listener, &master);
	memset(&buf[0], 0, sizeof(buf));

	for (;;) {

		read_fds = master;

		printf("En SelectPLP(): Waiting connection...\n");
		if (select(listener + 1, &read_fds, NULL, NULL, NULL ) == -1) {
			perror("selectPLP() salio mal...");
			exit(1);
		}

		socklen_t clilen;
		clilen = sizeof(clientaddr);
		if ((newfd = accept(listener, (struct sockaddr *) &clientaddr, &clilen))
				== -1) {
			perror("error en acceptPLP()...!");
		} else {
			printf("aceptado !...\n");

			if (recibirDatos(newfd, tam, (void*)&buf, logs)) {
				char* s = string_from_format("%s", &buf);
				log_debug(logs, "%s", &buf);
				registroPCB* unPCB = armarPCB(s,newfd);

				ponerCola(unPCB,NEW,mutexNEW, hayAlgo);


				if (write(newfd, "I got your message \n", 18) < 0)
					perror("ERROR writing to socket");
				memset(&buf[0], 0, sizeof(buf));
			}
		}
	}
	return 0;
}

int openSocketServerPCP(int PORT) {

	fd_set master;
	fd_set read_fds;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	int listener;
	int newfd;
	char buf[MAXBUFFSIZE];
	int nbytes;
	/* para setsockopt()  */
	int yes = 1;

	FD_ZERO(&master); //que se yo q hace pero tiene  estar
	FD_ZERO(&read_fds);

	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Server-socketPCP() error lol!");
		/*just exit lol!*/
		exit(1);
	}

	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
			== -1) {
		perror("errpr em setsockoptPCP!");
		exit(1);
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(PORT);   //config del server
	memset(&(serveraddr.sin_zero), '\0', 8);

	if (bind(listener, (struct sockaddr *) &serveraddr, sizeof(serveraddr))
			== -1) {
		perror("Bind salio mal!");
		exit(1);
	}

	if (listen(listener, 10) == -1) // LA MAXIMA CANTDAD DE CONEXIONES ES 10
			{
		perror("error en listenPCP()!");
		exit(1);
	}

	FD_SET(listener, &master);
	memset(&buf[0], 0, sizeof(buf));

	for (;;) {

		read_fds = master;

		printf("En SelectPCP(): Waiting connection...\n");
		if (select(listener + 1, &read_fds, NULL, NULL, NULL ) == -1) {
			perror("selectPCP() salio mal...");
			exit(1);
		}

		socklen_t clilen;
		clilen = sizeof(clientaddr);
		if ((newfd = accept(listener, (struct sockaddr *) &clientaddr, &clilen))
				== -1) {
			perror("error en acceptPCP()...!");
		} else {
			printf("aceptado !...\n");


				pthread_t thread;
				int iret1 = pthread_create(&thread, NULL, manejoCPU,(void*)newfd);

						if (iret1){
							log_info(logs, "Error en la creacion del hilo CPU");
							exit(EXIT_FAILURE);
						}
				if (write(newfd, "I got your CPU", 18) < 0)
					perror("ERROR writing to socket");
				memset(&buf[0], 0, sizeof(buf));


		}

	}
	return 0;
}





