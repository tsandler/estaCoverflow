/*
 * manejoDelPCB.c
 *
 *  Created on: 18/04/2014
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include "manejoDelPCB.h"

registroPCB pcb;

void inicializar(){
	pcb.primera_instruccion = 1;
}

void mostrar(){
	printf("%d", pcb.primera_instruccion);
}
