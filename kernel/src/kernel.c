/*
 * kernel.c
 *
 *  Created on: 18/06/2014
 *      Author: utnso
 */
#include "globales.h"
#include "planificador_corto_plazo.h"
#include "planificador_largo_plazo.h"

t_config *config;
t_queue *NEW;
t_queue *READY;
t_queue *EXIT;
sem_t mutexNEW;
sem_t mutexREADY;
sem_t gradoProg;
sem_t hayAlgo;
sem_t hayAlgoEnReady;
t_log *logs;

void plp(void* ptr);
void pcp(void* ptr);



int main(int argc, char *argv[]) {

/////////////////////////////////////////////////////////////////////////
	char* str2;
	str2 = "/home/utnso/git/tp-2014-1c-out-of-range/kernel/";
	char * str3 = (char *) malloc(
			1 + strlen("config.cnf") + strlen(str2));
	strcpy(str3, str2);
	strcat(str3, "config.cnf");

/////////////////////////////////////////////////////////////////////////

	config = config_create(str3);
	free(str3);
	NEW = queue_create();
	READY = queue_create();
	EXIT=queue_create();
	int gradoMultiprogramacion = config_get_int_value(config,"MULTIPROGRAMACION");
	sem_init(&mutexNEW, 0, 1);
	sem_init(&mutexREADY, 0, 1);
	sem_init(&gradoProg, 0, gradoMultiprogramacion-1);
	sem_init(&hayAlgo, 0, 0);
	sem_init(&hayAlgoEnReady,0,0);
	logs = log_create("log_Principal","kernel.c",0,LOG_LEVEL_TRACE);

	pthread_t thread1, thread2;

	int iret1, iret2;

	iret1 = pthread_create(&thread1, NULL, plp, NULL );

	if (iret1){
		log_error(logs,"Error en la creacion del hilo PLP");
		log_destroy(logs);

		exit(EXIT_FAILURE);

	}

	iret2 = pthread_create(&thread2, NULL, pcp, NULL );

	if (iret2){
		log_error(logs, "Error en la creacion del hilo PCP");
		log_destroy(logs);

		exit(EXIT_FAILURE);

	}

	pthread_join(thread1, NULL ); //esperar a q termine el otro

	pthread_join(thread2, NULL );

	sem_destroy(&hayAlgo);
	sem_destroy(&mutexNEW);
	sem_destroy(&mutexREADY);
	sem_destroy(&gradoProg);
	sem_destroy(&hayAlgoEnReady);

	exit(EXIT_SUCCESS);

	log_destroy(logs);
	config_destroy(config);

	return EXIT_SUCCESS;

}
