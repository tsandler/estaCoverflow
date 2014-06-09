/*
 * globales.h
 *
 *  Created on: 25/05/2014
 *      Author: utnso
 */
#ifndef GLOBALES_H_
#define GLOBALES_H_

#include <stdio.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <parser/metadata_program.h>
#include <commons/config.h>
#include <commons/log.h>
#include <libs/sockets.h>
#include <libs/manejoDelPCB.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


t_metadata_program programa;

t_length *tam;

typedef struct{
	t_config *config;
	t_log *logs;
	int socket_conectado;
	int bytesRecibidos;
	char* buffer;
	int error;
	t_metadata_program programa;
}PLP;

typedef struct{
	t_log *logs;
	int socket_conectado;
	char* buffer;
	int error;
	t_metadata_program programa;
}PCP;

t_list *colaNew;
t_list *colaReady;

sem_t *sem_multiprogramacion;

static pthread_mutex_t semaforo_mutex = PTHREAD_MUTEX_INITIALIZER;

#endif /* GLOBALES_H_ */
