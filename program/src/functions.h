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


void enviarProgramaAlKernel(char* programa);
char* recibirSentencia();
int conectarKernel();
void inicializarVariables();
int archivoDeConfiguracionValido();
void liberarEstructuras();

#endif /* FUNCTIONS_H_ */
