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
#include "globales.h"

int conectarUMV();
int conectarKernel();
int recibirQuantum();
int recibirTamanioStack();
int obtenerPosicion(char variable);
void liberarEstructuras();
char* recibirSentencia();
void pedirStack(int tamanio);
int archivoDeConfiguracionValido();
void manejarSenial();

#endif /* FUNCTIONS_H_ */
