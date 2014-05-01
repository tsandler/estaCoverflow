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

typedef struct{
	t_log *logs;
	int port;
}informacionUMV;

void *conectarUMV(void *);

int main(void) {
	informacionUMV var;
	t_config *config = config_create("config.cnf");
	var.logs = log_create("log", "CPU.c", 0, LOG_LEVEL_TRACE);
	pthread_t thr;

	int r1;
	var.port = config_get_int_value(config, "PORT");

	r1 = phtread_create(&thr, NULL, conectarUMV, (void*) var);

	if (r1 != 0){
		log_error(var.logs, "Se produjo un error en el hilo que se conecta con la UMV");
	}

	/*

	t_log *logs = log_create("log", "CPU.c", 0, LOG_LEVEL_TRACE);
	int s;
	char *ip = config_get_string_value(config, "IP");
	int port = config_get_int_value(config, "PORT");

	s = conectarCliente(ip, port, logs);
	if (s < 0){
		log_error(logs, "No se pudo conectar al servidor");
	}

	goto salto;

	printf("esto no corre");

	salto:


	config_destroy(config);*/
	return 0;
}

void *conectarUMV(void* var){
	informacionUMV *aux;
	aux = (informacionUMV*) var;
	crearServidor(aux->port, aux->logs);
	return (void*) aux;
}
