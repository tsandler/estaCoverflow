/*
 ============================================================================
 Name        : Kernel.c
 Author      : Hernan Suzuki Son
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include "plp.h"
#include "pcp.h"
#include <commons/config.h>
#include <commons/string.h>

#include <memory.h>
#include <stdlib.h>
#include <commons/collections/queue.h>
#include <semaphore.h>


t_config *config;
t_queue *NEW;
t_queue *READY;
sem_t mutexNEW;
sem_t mutexREADY;
sem_t gradoProg;
sem_t hayAlgo;

int main(int argc, char *argv[]) {

/////////////////////////////////////////////////////////////////////////
	char* str2;
	str2 = "/home/utnso/git/tp-2014-1c-vi-luz-y-entre/Kernel/src/";
	char * str3 = (char *) malloc(
			1 + strlen("kernelConfig.cfg") + strlen(str2));
	strcpy(str3, str2);
	strcat(str3, "kernelConfig.cfg");

/////////////////////////////////////////////////////////////////////////

	config = config_create(str3);
	free(str3);
	NEW = queue_create();
	READY = queue_create();
	int gradoMultiprogramacion = config_get_int_value(config,"MULTIPROGRAMACION");
	sem_init(&mutexNEW, 0, 1);
	sem_init(&mutexREADY, 0, 1);
	sem_init(&gradoProg, 0, gradoMultiprogramacion-1);
	sem_init(&hayAlgo, 0, 0);

	pthread_t thread1, thread2;

	int iret1, iret2;

	iret1 = pthread_create(&thread1, NULL, plp, NULL );

	if (iret1)

	{

		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret1);

		exit(EXIT_FAILURE);

	}

	iret2 = pthread_create(&thread2, NULL, pcp, NULL );

	if (iret2)

	{

		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret2);

		exit(EXIT_FAILURE);

	}

	pthread_join(thread1, NULL ); //esperar a q termine el otro

	pthread_join(thread2, NULL );

	sem_destroy(&hayAlgo);
	sem_destroy(&mutexNEW);
	sem_destroy(&mutexREADY);
	sem_destroy(&gradoProg);

	exit(EXIT_SUCCESS);

	return EXIT_SUCCESS;

}

