/*
 * globales.h
 *
 *  Created on: 18/05/2014
 *      Author: utnso
 */

#ifndef GLOBALES_H_
#define GLOBALES_H_

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <libs/sockets.h>
#include <libs/manejoDelPCB.h>
#include <commons/log.h>
#include <commons/config.h>

t_log *logs;
t_length* tam;
registroPCB *pcb;
t_config* config;
t_dictionary* diccionarioDeVariables;
int socketKernel, socketUMV;
unsigned char* stack;
int seguir;
bool systemCall;
int ejecutando;
int signalCall;
int tamanioStack;

AnSISOP_funciones functions;
AnSISOP_kernel kernel_functions;

#endif /* GLOBALES_H_ */
