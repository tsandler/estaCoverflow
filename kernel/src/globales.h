/*
 * globales.h
 *
 *  Created on: 18/06/2014
 *      Author: utnso
 */

#ifndef GLOBALES_H_
#define GLOBALES_H_

#include <stdio.h>
#include <pthread.h>
#include <memory.h>
#include <stdlib.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <semaphore.h>
#include <commons/log.h>
#include <pthread.h>
#include "planificador_largo_plazo.h"
#include "kernelSocket.h"
#include <commons/config.h>
#include <commons/string.h>
#include <libs/manejoDelPCB.h>
#include <libs/sockets.h>
#include "funcionesPCB.h"
#include "colas.h"

int conectarseUMV();
//static pthread_mutex_t semaforo_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct t_semaforos {
	int valor;
	t_queue *cola;
	sem_t hayAlgo;
	sem_t mutex;
} t_semaforos;



#endif /* GLOBALES_H_ */
