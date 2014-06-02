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
	int longitud;
	t_puntero posicion;
}t_variable;

typedef struct{
	t_nombre_compartida variable;
	t_valor_variable valor;
}t_variable_asignar;

typedef struct{
	t_puntero direccion_variable;
	t_valor_variable valor;
}t_val_variable;

typedef enum{
	DEFINIR_VARIABLE,
	OBTENER_POSICION_VARIABLE,
	DEREFERENCIAR,
	ASIGNAR,
	OBTENER_VALOR_COMPARTIDA,
	ASIGNAR_VALOR_COMPARTIDA,
	IR_AL_LABEL,
	LLAMAR_SIN_RETORNO,
	LLAMAR_CON_RETORNO,
	FINALIZAR,
	RETORNAR,
	IMPRIMIR,
	IMPRIMIR_TEXTO,
	ENTRADA_SALIDA,
	WAIT,
	SIGNAL,
	CONCLUYO_UN_QUANTUM,
	PEDIR_SEGMENTO,
	PEDIR_SENTENCIA,
	RESERVAR,
	ESCRIBIR,
}t_menu;

#endif /* ESTRUCTURASPARSER_H_ */
