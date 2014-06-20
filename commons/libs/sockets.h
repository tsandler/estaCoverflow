/*
 * sockets.h
 *
 *  Created on: 23/04/2014
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

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

typedef struct t_stream{
	int length;
	char* data;
}t_stream;

typedef struct{
	int base;
	int offset;
	int tamanio;
}t_etiqueta;

typedef enum{
	OBTENER_VALOR_COMPARTIDA,
	ASIGNAR_VALOR_COMPARTIDA,
	PEDIR_INDICE_ETIQUETAS,
	IMPRIMIR,
	IMPRIMIR_TEXTO,
	ENTRADA_SALIDA,
	WAIT,
	SIGNAL,
	PID_ACTUAL,
	CONCLUYO_UN_QUANTUM,
	FINALIZAR,
	LEER_SEGMENTO,
	PEDIR_SEGMENTO_CODIGO,
	PEDIR_SENTENCIA,
	RETORNO_DE_STACK,
	SOY_CPU,
	SOY_KERNEL,
}t_menu;

typedef struct t_length{
	t_menu menu; //valor para saber que se va a hacer con el dato enviado
	int length; //tamanio del dato enviado
}t_length;

int crearServidor(int port, t_log *logs);
int aceptarConexion(int socket, t_log *logs);
int enviarDatos(int socket, t_length* tam, void* datos, t_log* logs);
int enviarMenu(int socket, t_length* tam, t_log* logs);
int recibirMenu(int socket, t_length* tam, t_log* logs);
int recibirDato(int socket, int size, void** dato, t_log* logs);
int recibirDatos(int socket, t_length* tam, void** datos, t_log* logs);
int conectarCliente(char *ip, int port, t_log *logs);
void cerrarSocket(int socket);

#endif /* SOCKETS_H_ */
