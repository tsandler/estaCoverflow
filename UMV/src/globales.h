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
#include <unistd.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/error.h>
#include <libs/estructurasParser.h>


t_log *logs;
t_dictionary *tablaPidSeg;
t_list *listaHuecos;
char *ramUMV;
char *ramUMVinicial;
int tamanioUMV;
int retardoActual;
int algoritmo;
int pidActive;


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
	LEER_SEGMENTO=1,
	ESCRIBIR_SEGMENTO=2,
	CREAR_SEGMENTO=3,
	ELIMINAR_SEGMENTOS=4,
}t_operacion;

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
