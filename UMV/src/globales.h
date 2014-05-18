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
#include <commons/string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>

t_log *logs;
t_dictionary *tablaPidSeg;
t_list *listaDirLogica;
t_list *listaHuecos;
int baseLogica;
char *ramUMV;
char *ramUMVinicial;
int tamanioUMV;


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
}nodoDirLog;

typedef struct nodoHuecos{
	int dirLogica; //corroborar si es necesaria
	int tamanioSegmento;
	char *dirFisica;
}nodoHuecos;


int crear_agregar_segmento(int pid, int tamanio);


#endif /* GLOBALES_H_ */
