/*
 * kernelSocket.c
 *
 *  Created on: 18/06/2014
 *      Author: utnso
 */

#include "kernelSocket.h"
#include <commons/string.h>
#include "funcionesPCB.h"


#define BROKEN "BROKEN"
#define GREAT "GREAT"


extern t_queue *NEW;
extern sem_t mutexNEW;
extern sem_t hayAlgo;
extern t_log*logs;
extern int socket_UMV;

int openSocketServerPLP(int PORT) {

	fd_set master;
	fd_set read_fds;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	int listener;
	int newfd;
	char* buf;

	t_length* tam=malloc(sizeof(t_length));
	/* para setsockopt()  */
	int yes = 1;

	FD_ZERO(&master); //que se yo q hace pero tiene  estar
	FD_ZERO(&read_fds);

	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		log_error(logs,"Error en el Server-Socket-PLP");
		/*just exit lol!*/
		exit(EXIT_FAILURE);//exit(1);
	}

	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
			== -1) {
		log_error(logs,"Error en SET socket open PLP");
		exit(EXIT_FAILURE);//exit(1);
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(PORT);   //config del server
	memset(&(serveraddr.sin_zero), '\0', 8);

	if (bind(listener, (struct sockaddr *) &serveraddr, sizeof(serveraddr))
			== -1) {
		log_error(logs,"El BIND del PLP salio mal");
		exit(EXIT_FAILURE);//exit(1);
	}

	if (listen(listener, 10) == -1){ // LA MAXIMA CANTDAD DE CONEXIONES ES 10

		log_error(logs,"Ocurrio un error mientras el PLP hacia LISTEN");
		exit(EXIT_FAILURE);
		//exit(1);
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
		printf("En PLP(): Waiting conection ...\n");
		log_info(logs,"En PLP(): Waiting Connection...\n");
		socklen_t clilen;
		clilen = sizeof(clientaddr);
		if ((newfd = accept(listener, (struct sockaddr *) &clientaddr, &clilen))
				== -1) {
			log_error(logs,"Error cuando el PLP tenia que aceptar lo recibido");
			exit(EXIT_FAILURE);
		} else {
			log_info(logs, "Conexion entrante aceptada :%i",clilen);
			if (recibirDatos(newfd, tam, (void*)&buf, logs)==1) {
				puts("llego");
				log_info(logs,"%s \n",buf);
				registroPCB* unPCB = malloc(sizeof(registroPCB));
				unPCB = armarPCB(buf,newfd);
				if(unPCB->peso==-1){
					eliminarSegmentoUMV(socket_UMV,logs,unPCB);
					tam->menu=MEMORY_OVERLOAD;
					enviarMenu(newfd,tam,logs);
					log_info(logs,"La UMV se quedo sin memoria");
				}else {

				ponerCola(unPCB,NEW,&mutexNEW, &hayAlgo);
				muestraNombres(NEW,"NEW");
				log_info(logs,"Se coloco en la cola NEW el programa %i",unPCB->pid);
				}



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
	/* para setsockopt()  */
	int yes = 1;

	FD_ZERO(&master); //que se yo q hace pero tiene  estar
	FD_ZERO(&read_fds);

	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		log_error(logs,"Error en Server-Socket PCP()");
		/*just exit lol!*/
		exit(EXIT_FAILURE);//exit(1);
	}

	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
			== -1) {
		log_error(logs,"Error en el Set sockopt PCP");
		exit(EXIT_FAILURE);//exit(1);
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(PORT);   //config del server
	memset(&(serveraddr.sin_zero), '\0', 8);

	if (bind(listener, (struct sockaddr *) &serveraddr, sizeof(serveraddr))
			== -1) {
		log_error(logs,"Error en el BIND");
		exit(EXIT_FAILURE);//exit(1);
	}

	if (listen(listener, 10) == -1) // LA MAXIMA CANTDAD DE CONEXIONES ES 10
			{
		log_error(logs,"Error en el listen del PCP");
		exit(EXIT_FAILURE);//exit(1);
	}

	FD_SET(listener, &master);


	for (;;) {

		read_fds = master;
		printf("En PCP(): Waiting conection ...\n");
		log_info(logs,"En PCP(): Waiting conection ...");
		if (select(listener + 1, &read_fds, NULL, NULL, NULL ) == -1) {
			log_error(logs,"El select del PCP salio mal...");
			exit(EXIT_FAILURE);//exit(1);
		}

		socklen_t clilen;
		clilen = sizeof(clientaddr);

		if ((newfd = accept(listener, (struct sockaddr *) &clientaddr, &clilen))
				== -1) {
			log_error(logs,"Error en cuando el PCP queria aceptar la conexion");
			exit(EXIT_FAILURE);//tendria que poner esto aca o no se pone nada y se permite que siga ejecutando
		} else {

			// log_info(logs, "aceptado !..."); este log es innecesario
				pthread_t thread;
				int iret1 = pthread_create(&thread, NULL, manejoCPU,(void*)newfd);

				if (iret1){
					log_info(logs, "Error en la creacion del hilo CPU");
					exit(EXIT_FAILURE);
				}

				pthread_join(iret1, NULL);
		}


	}
	return 0;
}

