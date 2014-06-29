/*
 * planificador_largo_plazo.c
 *
 *  Created on: 18/06/2014
 *      Author: utnso
 */
#include "globales.h"

extern t_queue *NEW;
extern t_queue *READY;
extern t_queue * EXIT;
extern sem_t mutexNEW;
extern sem_t mutexREADY;
extern sem_t mutexEXIT;

extern sem_t gradoProg;
extern sem_t hayAlgo;
extern sem_t hayAlgoEnReady;
extern sem_t hayAlgoEnExit;
extern t_log *logs;
extern t_config *config;

extern int socketUMV;

/*
 *
 * saca el pcb de peso menor. para el SNF
 *
 */

registroPCB *queue_pop_min(t_queue *self) {
	int tamanio=queue_size(self);
	registroPCB* minPCB = malloc(sizeof(registroPCB));
	minPCB=queue_pop(self);


	log_info(logs,"Se saco el programa segun SJN");
	int i=1;
	while(i<tamanio){
		registroPCB* otroPCB =malloc(sizeof(registroPCB));
		puts("otropcb");
		otroPCB=queue_pop(self);
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
	puts("Corriendo hilo de New a Ready \n");
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
			sem_post(&hayAlgoEnReady);
			sem_post(&mutexREADY);

	}

}

void manejoExit(){
	puts("Corriendo Hilo manejoExit \n");
	registroPCB * unPCB= malloc(sizeof(registroPCB));

	while(1){

		unPCB=sacarCola(EXIT,&mutexEXIT,&hayAlgoEnExit);
		//eliminarSegmentoUMV(socketUMV,  logs,unPCB);
		sem_post(&gradoProg);
	}

}


void plp(void* ptr){

	pthread_t thread4;
	pthread_t thread5;
	pthread_t thread6;
	int port=config_get_int_value(config, "PUERTO_PROG");




	int iret1 = pthread_create(&thread4, NULL, (void*)openSocketServerPLP,(void*)port);

		if (iret1){
			log_info(logs, "Error en la creacion del hilo openSocketServerPLP");
			exit(EXIT_FAILURE);
		}

	int iret2 = pthread_create(&thread5, NULL, (void*)deNewAReady,NULL);

		if (iret2){
			log_info(logs,"Error en la creacion del hilo deNewAReady");
			exit (EXIT_FAILURE);
		}

		int iret3 = pthread_create(&thread6, NULL, (void*)manejoExit,NULL);

			if (iret3){
				log_info(logs,"Error en la creacion del hilo manejoExit");
				exit (EXIT_FAILURE);
			}

		pthread_join(thread4, NULL ); //esperar a q termine el otro

		pthread_join(thread5, NULL );
		pthread_join(thread6, NULL );

}

