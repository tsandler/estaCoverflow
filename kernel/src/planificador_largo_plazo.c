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


extern int socket_UMV;

/*
 *
 * saca el pcb de peso menor. para el SNF
 *
 */

registroPCB *queue_pop_min(t_queue *self) {
	int tamanio=queue_size(self);
	registroPCB* minPCB = malloc(sizeof(registroPCB));
	minPCB=queue_pop(self);


	/* LOS DOS LOG SON INNECESARIOS CREO QUE IGUAL ES MEJOR CHARLALO ANTES DE BORRARLO DEFINITIVAMENTE*/
	//log_info(logs,"Se saco el programa %i de la cola",minPCB->pid);
	//log_info(logs,"Se saco el programa segun SJN");

	int i=1;
	while(i<tamanio){
		registroPCB* otroPCB =malloc(sizeof(registroPCB));

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


void deNewAReady(){
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
			muestraNombres(NEW,"NEW");
			//log_info(logs,"El pop saco el peso %d \n", unPCB->peso); este log es innecesario
			//log_info(logs,"En la cola NEW hay %d \n",queue_size(NEW));este log es tambien innecesario
			sem_post(&mutexNEW);

			sem_wait(&mutexREADY);
			muestraNombres(READY,"READY");
			log_info(logs,"En la cola Ready Hay %i \n", queue_size(READY));
			queue_push(READY,unPCB);
			muestraNombres(READY,"READY");
			log_info(logs,"En la cola Ready se agrego el programa %d, ahora hay un total de %i \n",unPCB->pid,queue_size(READY));
			sem_post(&hayAlgoEnReady);
			sem_post(&mutexREADY);

	}

}

void manejoExit(){/*
	puts("Corriendo Hilo manejoExit \n");
	registroPCB * unPCB= malloc(sizeof(registroPCB));

	while(1){

		unPCB=sacarCola(EXIT,&mutexEXIT,&hayAlgoEnExit);
		eliminarSegmentoUMV(socket_UMV,  logs,unPCB);
		sem_post(&gradoProg);
	}*/

}



