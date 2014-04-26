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
	int bytesRecibidos;
	struct sockaddr_in dir;
	char buffer[BUFF_SIZE];

	dir.sin_family = PF_INET;
	dir.sin_port = htons(port);
	dir.sin_addr.s_addr = htonl(INADDR_ANY);

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s < 0){
		log_error(logs, "No se pudo conectar con el socket.");
		perror("Error al conectar socket");
		return 0;
	}

	if (bind(s, (struct sockaddr*)&dir, sizeof(dir)) != 0){
		log_error(logs, "Error al bindear el socket.");
		perror("Error al bindear el socket");
		return 0;
	}

	if (listen(s, SOMAXCONN) != 0) {
		log_error(logs, "Error al escuchar el socket.");
		perror("Error al poner a escuchar socket");
		return 0;
	}

	if ((socket_conectado = accept(s, NULL, 0)) < 0) {
		log_error(logs, "Error al aceptar conexion entrante");
		perror("Error al aceptar conexion entrante");
		return 0;
	}

	while (1) {
		bytesRecibidos = recv(socket_conectado, buffer, BUFF_SIZE, 0);
		if (bytesRecibidos >= 0) {
			printf("Mensaje recibido: ");
			fwrite(buffer, 1, bytesRecibidos, stdout);
			printf("\n");
			printf("Tamanio del buffer %d bytes! \n", bytesRecibidos);
			fflush(stdout);

			if (memcmp(buffer, "fin", bytesRecibidos) == 0) {
				log_info(logs, "Servidor cerrado correctamente");
				printf("Server cerrado correctamente.\n");
				break;
			}
		}else{
			log_error(logs, "Error al recibir los datost.");
			perror("Error al recibir datos");
			return 0;
		}
	}

	close(s);
	close(socket_conectado);
	return 1;
}


/* Funcion para conectar al cliente con el servidor y enviar datos, recibe los
   datos de conexion y la informacion a enviar, devuelve 0 en caso de error */

int conectarCliente(char *ip, int port, char *script, t_log *logs){
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
		return 0;
	}
	printf("Conectado!\n");

	while (1) {
		if (send(s, script, strlen(script), 0) >= 0) {
			printf("Datos enviados!\n");
			if (strcmp(script, "fin") == 0) {
				log_info(logs, "Cliente cerrado correctamente.");
				printf("Cliente cerrado correctamente.\n");
				break;
			}

		} else {
			log_error(logs, "Error al enviar datos, server no encontrado.");
			perror("Error al enviar datos. Server no encontrado.\n");
			break;
		}
		scanf("%s", script);
	}
	close(s);
	return 1;
}
