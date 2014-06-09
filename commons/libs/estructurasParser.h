/*
 * estructurasParser.h
 *
 *  Created on: 23/05/2014
 *      Author: utnso
 */

#ifndef ESTRUCTURASPARSER_H_
#define ESTRUCTURASPARSER_H_

#include "../parser/parser.h"

typedef struct{
	int base;
	int offset;
	int tamanio;
}t_etiqueta;

typedef enum{
	OBTENER_VALOR_COMPARTIDA,
	ASIGNAR_VALOR_COMPARTIDA,
	PEDIR_INDICE_ETIQUETAS,
	IMPRIMIR,
	IMPRIMIR_TEXTO,
	ENTRADA_SALIDA,
	WAIT,
	SIGNAL,
	CONCLUYO_UN_QUANTUM,
}t_menu;

#endif /* ESTRUCTURASPARSER_H_ */
