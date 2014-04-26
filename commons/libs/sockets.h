/*
 * sockets.h
 *
 *  Created on: 23/04/2014
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

#define BUFF_SIZE 1024

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <unistd.h>

#include "../commons/config.h"
#include "../commons/log.h"

int crearServidor(int port, t_log *logs);
int conectarCliente(char *ip, int port, char *script, t_log *logs);

#endif /* SOCKETS_H_ */
