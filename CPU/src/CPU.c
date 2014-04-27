/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <libs/sockets.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>

typedef struct{
	t_log *logs;
	int port;
}informacionUMV;

void *conectarUMV(void *);

int main(void) {
	/*informacionUMV var;
	var.logs = log_create("log", "CPU.c", 0, LOG_LEVEL_TRACE);
	t_config *config = config_create("config.cnf");
	pthread_t thr;

	int r1;
	var.port = config_get_int_value(config, "PORT");

	r1 = phtread_create(&thr, NULL, conectarUMV, (void*) var);

	if (r1 != 0){
		log_error(logs, "Se produjo un error en el hilo que se conecta con la UMV")
	}
	log_destroy(var.logs);
	config_destroy(config);*/
	t_log *logs = log_create("log", "CPU.c", 0, LOG_LEVEL_TRACE);
	t_config *config = config_create("config.cnf");

	int port = config_get_int_value(config, "PORT");
	int bytesRecibidos;
	char buffer[1024];
	int socket_conectado = crearServidor(port, logs);
	if(socket_conectado < 0 ){
		log_error(logs, "El servidor no se creo correctamente");
		perror("El proceso no se realizo correctamente.");
		return 0;
	}
	while (1){
		bytesRecibidos=recv(socket_conectado, buffer, 1024, 0);
		if(bytesRecibidos >= 0){
			printf("Recibio: %s\n", buffer);
			if (memcmp(buffer, "fin", bytesRecibidos) == 0){
				break;
			}
		}else{
			printf("Hubo un error");
			break;
		}
	}
	log_info(logs, "El servidor se creo correctamente");
	puts("El proceso se realizo correctamente.");
	log_destroy(logs);
	config_destroy(config);
	close(socket_conectado);
	return 0;
}

void *conectarUMV(void* var){
	informacionUMV *aux;
	aux = (informacionUMV*) var;
	crearServidor(aux->port, aux->logs);
	return 0;
}
