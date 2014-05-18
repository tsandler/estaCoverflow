/*
 ============================================================================
 Name        : program.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <libs/sockets.h>
#include <commons/config.h>
#include <commons/log.h>

int main(int index, char **argcv) {
	t_log *logs = log_create("log", "program.c", 0, LOG_LEVEL_TRACE);
	t_config *config = config_create("config.cnf");
	char buffer[1024];
	char *ip = config_get_string_value(config, "IP");
	int port = config_get_int_value(config, "PUERTO_KERNEL");
	int s;

	if (index != 2){
		log_error(logs, "No se envio un parametro al proceso program");
		perror("No se envio un parametro para procesar");
		return 0;
	}
	s = conectarCliente(ip, port, logs);
	//Posicion 0 es el mismo programa y 1 es el script ansisop enviado por parametro
	if (s < 0){
		log_error(logs, "El cliente no se pudo conectar correctamente");
		perror("El cliente no se pudo conectar");
		log_destroy(logs);
		config_destroy(config);
		return 0;
	}
	while (1) {
		if (send(s, argcv[1], strlen(argcv[1]) + 1, 0) >= 0){
			printf("Script enviado!\n");
			break;
		} else {
			log_error(logs, "Error al enviar datos, server no encontrado.");
			perror("Error al enviar datos. Server no encontrado.\n");
			goto endProgram;
			break;
		}
	}
	while(1){
		if (recv(s, buffer, 1024,0)){
			break;
		}else{
			log_error(logs, "Se produjo un error recibiendo los datos del kernel");
			return 0;
		}
	}

	printf("Datos recibidos del kernel:\n%s", buffer);

	log_info(logs, "El cliente se conecto y envio la informacion correctamente");
	puts("El cliente envio la informacion correctamente.");

	endProgram:
	config_destroy(config);
	log_destroy(logs);
	close(s);
	return 0;
}