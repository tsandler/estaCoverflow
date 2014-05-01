/*
 * sockets.c
 *
 *  Created on: 23/04/2014
 *      Author: utnso
 */

#include "sockets.h"

/* Funcion para crear el servidor y procesar los datos que son enviados
por el cliente, devuelve 0 en caso de error. */

int crearServidor(int port, t_log *logs){
	int s, socket_conectado;
	struct sockaddr_in dir;

	dir.sin_family = PF_INET;
	dir.sin_port = htons(port);
	dir.sin_addr.s_addr = htonl(INADDR_ANY);

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s < 0){
		log_error(logs, "No se pudo conectar con el socket.");
		perror("Error al conectar socket");
		return -1;
	}

	if (bind(s, (struct sockaddr*)&dir, sizeof(dir)) != 0){
		log_error(logs, "Error al bindear el socket.");
		perror("Error al bindear el socket");
		close(s);
		return -1;
	}

	if (listen(s, SOMAXCONN) != 0) {
		log_error(logs, "Error al escuchar el socket.");
		perror("Error al poner a escuchar socket");
		close(s);
		return -1;
	}

	if ((socket_conectado = accept(s, NULL, 0)) < 0) {
		log_error(logs, "Error al aceptar conexion entrante");
		perror("Error al aceptar conexion entrante");
		close(s);
		return -1;
	}

	close(s);
	return socket_conectado;
}

/* Funcion para conectar al cliente con el servidor y enviar datos, recibe los
   datos de conexion y la informacion a enviar, devuelve 0 en caso de error */

int conectarCliente(char *ip, int port, t_log *logs){
	int s;
	struct sockaddr_in dir;

	printf("Conectando...\n");

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s < 0){
		log_error(logs, "No se pudo crear el socket");
		perror("Error al crear socket");
		return 0;
	}
	dir.sin_family = PF_INET;
	dir.sin_port = htons(port);
	dir.sin_addr.s_addr = inet_addr(ip);
	if (connect(s, (struct sockaddr*) &dir, sizeof(dir))!= 0) {
		log_error(logs, "Error al conectar socket");
		perror("Error al conectar socket");
		return -1;
	}
	printf("Conectado!\n");

	return s;
}
