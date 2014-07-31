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
#include <semaphore.h>
#include <commons/collections/queue.h>


pthread_t pthread_kernel, pthread_consola;
pthread_t pthread_CPU;
t_log* logs;
t_dictionary* tablaPidSeg;
t_list* listaHuecos;
t_config* config;
t_length* tam;
t_queue* colaKernel;
t_list* colaCPUs;
FILE* reporteEstructuras;
FILE* resultadoConsola;

int tamanioUMV;
int retardoActual;
int algoritmoActual;
int puerto;
int segEscritos;

// globales para compactar
char* ramUMVInicial;
char* ramAux;
char* dirHueco;
int tamHueco;
int encontroHueco;


typedef struct{
	t_length* tama;
}t_elem_cola_kernel;

typedef struct{
	int socket;
	t_length* tama;
}t_elem_cola_cpu;

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
	COMPACTAR_MEMORIA = 4,
	DUMP = 5,
	LEER_SEG = 1,
	ESCR_SEG = 2,
	CREAR_SEG = 3,
	ELIM_SEG = 4,
}t_op_consola;

typedef enum{
	ESTRUCTURAS_MEMORIA = 1,
	MEMORIA_PRINCIPAL = 2,
	CONTENIDO_MEM_PPAL = 3,
}t_op_dump;

typedef enum{
	GENERAR_ARCHIVO= 1,
}t;


#endif /* GLOBALES_H_ */
