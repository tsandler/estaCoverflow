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

int openSocketServerPLP(int PORT) {

	fd_set master;
	fd_set read_fds;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	int listener;
	int newfd;
	char buf[MAXBUFFSIZE];

	int nbytes;
	t_length* tam=malloc(sizeof(t_length));
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
		perror("error en setsockoptPCP!");
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


	for (;;) {

		read_fds = master;

/*		printf("En SelectPLP(): Waiting connection...\n");
		if (select(listener + 1, &read_fds, NULL, NULL, NULL ) == -1) {    //QUE HACEMOS CON ESTO???//
			perror("selectPCP() salio mal...");
			exit(1);
		}
*/
		log_info(logs,"En PLP(): Waiting Connection...");
		socklen_t clilen;
		clilen = sizeof(clientaddr);
		if ((newfd = accept(listener, (struct sockaddr *) &clientaddr, &clilen))
				== -1) {
			perror("error en acceptPLP()...!");
		} else {
			log_info(logs, "Conexion entrante aceptada :&i",clilen);
			if (recibirDatos(newfd, tam, (void*)&buf, logs)==1) {
				puts("llego");
				log_info(logs,"%s",&buf);
				registroPCB* unPCB = armarPCB(&buf,newfd);

				ponerCola(unPCB,NEW,&mutexNEW, &hayAlgo);



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
	char *buf;
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
		perror("error en setsockoptPCP!");
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


	for (;;) {

		read_fds = master;

		log_info(logs,"En PCP(): Waiting conection ...");
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
			// log_info(logs, "aceptado !..."); este log es innecesario

				pthread_t thread;
				int iret1 = pthread_create(&thread, NULL, manejoCPU,(void*)newfd);

						if (iret1){
							log_info(logs, "Error en la creacion del hilo CPU");
							exit(EXIT_FAILURE);
						}



		}

	}
	return 0;
}

