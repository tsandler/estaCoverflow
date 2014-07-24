/*
 * planificador_corto_plazo.c
 *
 *  Created on: 18/06/2014
 *      Author: utnso
 */


#include "planificador_corto_plazo.h"
#include "manejoCPU.h"
extern t_config *config;
//extern t_queue *NEW;
//extern t_queue *READY;
//extern t_queue *EXIT;
extern t_log *logs;

void pcp(void* ptr) {

	pthread_t thread3;//, thread4;
	int port = config_get_int_value(config, "PUERTO_CPU");

	int iret3 = pthread_create(&thread3, NULL, (void*)openSocketServerPCP,(void*)port);

	if (iret3) {
		log_info(logs,"Error en la creacion del hilo serverpcp");
		exit(EXIT_FAILURE);
	}





}

