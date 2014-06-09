/*
 * globales.h
 *
 *  Created on: 18/05/2014
 *      Author: utnso
 */

#ifndef GLOBALES_H_
#define GLOBALES_H_

#include <stdio.h>
#include <libs/sockets.h>
#include <libs/manejoDelPCB.h>
#include <commons/log.h>
#include <commons/config.h>

t_log *logs;
t_length* tam;
registroPCB *pcb;
t_config* config;
t_dictionary* diccionarioDeVariables;
int socket_kernel, socket_umv;
unsigned char* stack;
int desplazamiento;
bool systemCall;
int error; //TODO: no se si hace falta // variable de error por si el programa tiene que cortar la ejecucion

AnSISOP_funciones functions;
AnSISOP_kernel kernel_functions;

#endif /* GLOBALES_H_ */
