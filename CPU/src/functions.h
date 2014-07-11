/*
 * functions.h
 *
 *  Created on: 03/06/2014
 *      Author: utnso
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <stdio.h>
#include <commons/config.h>
#include <commons/string.h>
#include "globales.h"

int conectar_UMV();
int conectar_kernel();

void cargar_diccionario();
int recibir_quantum();
int recibir_tamanio_stack();
int recibir_retardo();
void pedir_stack(int tamanio);
char* recibir_sentencia();

int archivo_de_configuracion_valido();
void manejar_senial();
void liberar_estructuras();

#endif /* FUNCTIONS_H_ */
