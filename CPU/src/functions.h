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
int recibir(int valor);
char* recibir_sentencia();

int archivo_de_configuracion_valido();
void vaciarDiccionario();
void manejar_senial();
void liberar_estructuras();

#endif /* FUNCTIONS_H_ */
