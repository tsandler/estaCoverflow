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
#include <commons/collections/dictionary.h>
#include <parser/parser.h>
#include "funcionesParser.h"

t_log *logs;
t_length* tam;
registroPCB *pcb;
int socket_kernel, socket_umv;
char* sentencia;
t_dictionary* diccionarioDeVariables;

#endif /* GLOBALES_H_ */
