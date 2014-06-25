/*
 * functions.h
 *
 *  Created on: 24/05/2014
 *      Author: utnso
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <sys/stat.h>
#include "globales.h"
#include <commons/config.h>
#include <commons/string.h>


void enviar_programa_al_kernel(char* programa);
char* recibir_sentencia();
int conectar_kernel();
void inicializar_variables();
int archivo_de_configuracion_valido();
void liberar_estructuras();

#endif /* FUNCTIONS_H_ */
