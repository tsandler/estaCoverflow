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

int main(void) {
	t_log *logs = log_create("log", "CPU.c", 0, LOG_LEVEL_TRACE);
	t_config *config = config_create("config.cnf");
	int port = config_get_int_value(config, "PORT");

	if(!crearServidor(port, logs)){
		log_error(logs, "El servidor no se creo correctamente");
		perror("El proceso no se realizo correctamente.");
		return 0;
	}
	log_info(logs, "El servidor se creo correctamente");
	puts("El proceso se realizo correctamente.");
	log_destroy(logs);
	config_destroy(config);
	return 0;
}
