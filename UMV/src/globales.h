/*
 * globales.h
 *
 *  Created on: 14/05/2014
 *      Author: utnso
 */

#ifndef GLOBALES_H_
#define GLOBALES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/txt.h>
#include <libs/sockets.h>

pthread_t pthread_kernel, pthread_CPU, pthread_consola;
t_log *logs;
t_dictionary *tablaPidSeg;
t_list *listaHuecos;
t_config* config;
t_length* tam;
int tamanioUMV;
int retardoActual;
int algoritmoActual;
int pidActive;
int puerto;


typedef struct{
	int socket;
}estructura_hilo;

typedef struct{
	int base;
	int offset;
	int tamanio;
}datos_acceso;

typedef struct{
	int tamanio;
	int pid;
}datos_crearSeg;


typedef struct tablaSegUMV{
	int idSegmento;
	int dirLogica;
	int tamanioSegmento;
	char *dirFisica;
}tablaSegUMV;

typedef struct nodoDirLog{
	int dirLog;
	char *dirFisica;
	int tamanio;
	char *pid;
}nodoDirLog;

typedef struct nodoHuecos{
	int tamanioSegmento;
	char *dirFisica;
}nodoHuecos;

typedef enum{
	FIRST_FIT = 0,
	WORST_FIT = 1
}t_algoritmo;


typedef enum{
	CAMBIO_PROCESO_ACTIVO= 0,
	OPERACION = 1,
	CAMBIAR_RETARDO = 2,
	CAMBIAR_ALGORITMO = 3,
	COMPACTACION = 4,
	DUMP = 5,
}t_op_consola;

typedef enum{
	ESTRUCTURAS_MEMORIA = 0,
	MEMORIA_PRINCIPAL = 1,
	CONTENIDO_MEM_PPAL = 2,
}t_op_dump;

typedef enum{
	GENERAR_ARCHIVO= 1,
}t;


#endif /* GLOBALES_H_ */
