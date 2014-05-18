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

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <unistd.h>

#include "../commons/config.h"
#include "../commons/log.h"

typedef struct{
	int menu; //valor para saber que se va a hacer con el dato enviado
	int length; //tamanio del dato enviado
}t_length;

int crearServidor(int port, t_log *logs);
int aceptarConexion(int s, t_log *logs);
int enviarDatos(int s, t_length* tam, void* datos, t_log* logs);
int recibirDatos(int s, t_length* tam, void** datos, t_log* logs);
int conectarCliente(char *ip, int port, t_log *logs);
void cerrarSocket(int s);

#endif /* SOCKETS_H_ */
