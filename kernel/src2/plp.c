#include "plp.h"
#include "sockets.h"
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/queue.h>
#include "pcb.h"
extern t_config *config;
extern t_queue *NEW;
extern t_queue *READY;
extern sem_t mutexNEW;
extern sem_t mutexREADY;
extern sem_t gradoProg;
extern sem_t hayAlgo;



/*
 *
 * saca el pcb de peso menor. para el SNF
 *
 */

t_pcb *queue_pop_min(t_queue *self) {
	int tamanio=queue_size(self);
	t_pcb* minPCB=queue_pop(self);
	printf ("el pop saco el peso %d \n", minPCB->peso);
	int i=1;
	while(i<tamanio){
		t_pcb* otroPCB=queue_pop(self);
	if(otroPCB->peso>minPCB->peso){
		queue_push(otroPCB,self);
	}
	else {
		queue_push(minPCB,self);
		minPCB=otroPCB;
	}
	i=i+1;
	}
	return minPCB;


}



void plp(void* ptr){

	pthread_t thread1;
	int port=config_get_int_value(config, "PUERTO_PROG");




	int iret1 = pthread_create(&thread1, NULL, openSocketServerPLP,(void*)port);

		if (iret1)

		{

			fprintf(stderr, "Error - pthread_create() return code: %d\n", iret1);

			exit(EXIT_FAILURE);


		}
		while(1){

			sem_wait(&hayAlgo);
			sem_wait(&gradoProg);
			sem_wait(&mutexNEW);

			int value;
			sem_getvalue(&gradoProg,&value);
			printf("gradoprog = %d \n",value);
			printf("en la cola New HAY %d \n", queue_size(NEW));
			t_pcb* unPCB=queue_pop_min(NEW);
			printf ("el pop saco el peso %d \n", unPCB->peso);
			printf("en la cola DESPUES New HAY %d \n", queue_size(NEW));
			sem_post(&mutexNEW);

			sem_wait(&mutexREADY);

//			printf("en la cola READY HAY %d \n", queue_size(READY));
			queue_push(READY,unPCB);
			printf("en la cola DESPUES READY HAY %d \n", queue_size(READY));

			sem_post(&mutexREADY);



		}



}
