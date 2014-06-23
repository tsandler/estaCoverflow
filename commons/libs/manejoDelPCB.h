/*
 * manejoDelPCB.h
 *
 *  Created on: 24/05/2014
 *      Author: utnso
 */

#ifndef MANEJODELPCB_H_
#define MANEJODELPCB_H_

#include <stdio.h>
#include <stdlib.h>
#include "../parser/metadata_program.h"



typedef struct registroPCB{
    int pid;

	int* segemento_stack;
	int* cursor_stack;
	int tamanio_contexto;

	int* segemento_codigo;
	int tamanio_indice_codigo ;
	t_intructions* indice_codigo;
	int program_counter;

	char* indice_etiquetas_funciones;
	int* puntero_etiquetas_funciones;
	int tamanio_indice_etiquetas_funciones;

	int retrasoIO;
	int peso;
	int fd;

}registroPCB;

registroPCB* armarPCB(char* program, int fd);

#endif /* MANEJODELPCB_H_ */
