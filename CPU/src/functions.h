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

int conectarUMV();
int conectarKernel();

void cargarDiccionario();
int recibirQuantum();
int recibirTamanioStack();
void pedirStack(int tamanio);
char* recibirSentencia();
char* depurarSentencia();

int archivoDeConfiguracionValido();
void manejarSenial();
void liberarEstructuras();

#endif /* FUNCTIONS_H_ */
