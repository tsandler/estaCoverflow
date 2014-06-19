/*
 * planificador_largo_plazo.c
 *
 *  Created on: 18/06/2014
 *      Author: utnso
 */
#include "globales.h"

extern t_queue *NEW;
extern t_queue *READY;
extern sem_t mutexNEW;
extern sem_t mutexREADY;
extern sem_t gradoProg;
extern sem_t hayAlgo;
extern t_log *logs;
extern t_config *config;

/*
 *
 * saca el pcb de peso menor. para el SNF
 *
 */

registroPCB *queue_pop_min(t_queue *self) {
	int tamanio=queue_size(self);
	registroPCB* minPCB = queue_pop(self);

	log_info(logs,"Se saco el programa seg{un SJN");
	int i=1;
	while(i<tamanio){
		registroPCB* otroPCB = queue_pop(self);
	if(otroPCB->peso > minPCB->peso){
		queue_push(self,otroPCB);
	}
	else {
		queue_push(self,minPCB);
		minPCB=otroPCB;
	}
	i=i+1;
	}
	return minPCB;


}


void deNewAReady(void){
	while(1){

			sem_wait(&hayAlgo);
			sem_wait(&gradoProg);
			sem_wait(&mutexNEW);

			int value;

			sem_getvalue(&gradoProg,&value);
			log_info(logs, "El grado de multiprogramacion es : %i \n",value);
			log_info(logs,"En la cola New hay: %i \n",queue_size(NEW));
			registroPCB* unPCB=queue_pop_min(NEW);
			log_info(logs,"El pop saco el peso %d \n", unPCB->peso);
			log_info(logs,"En la cola NEW hay %d \n",queue_size(NEW));
			sem_post(&mutexNEW);

			sem_wait(&mutexREADY);

			log_info(logs,"En la cola Ready Hay %i \n", queue_size(READY));
			queue_push(READY,unPCB);
			log_info(logs,"En la cola Ready Hay %d \n",queue_size(READY));

			sem_post(&mutexREADY);

	}

}


void plp(void* ptr){

	pthread_t thread4;
	pthread_t thread5;
	int port=config_get_int_value(config, "PUERTO_PROG");




	int iret1 = pthread_create(&thread4, NULL, openSocketServerPLP,(void*)port);

		if (iret1){
			log_info(logs, "Error en la creacion del hilo openSocketServerPLP");
			exit(EXIT_FAILURE);
		}

	int iret2 = pthread_create(&thread5, NULL, deNewAReady,(void*)port);

		if (iret2){
			log_info(logs,"Error en la creacion del hilo deNewAReady");
			exit (EXIT_FAILURE);
		}
		pthread_join(thread4, NULL ); //esperar a q termine el otro

		pthread_join(thread5, NULL );

}


void conectarseUMV(){
    int socket_UMV;
    char *ip = config_get_string_value(config,"IP");
    int port = config_get_int_value(config,"PUERTO_UMV");
    socket_UMV = conectarCliente(ip, port, logs);
	if(socket_UMV < 0){
		log_error(logs, "El cliente no se pudo conectar correctamente");
		log_destroy(logs);
	}else{
		log_info(logs, "El kernel se conecto correctamente con la UMV");
	}
} // El socket de la uMV habria que cerrarlo una vez que se resfieren los datos y todo...
