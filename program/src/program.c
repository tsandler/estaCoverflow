/*
 ============================================================================
 Name        : program.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "globales.h"
#include "functions.h"

int main(int argc, char **argv) {

	t_config *config = config_create(argv[1]);

	char *ip = config_get_string_value(config, "IP");
	int port = config_get_int_value(config, "PUERTO_KERNEL");

	config_destroy(config);

	logs = log_create("log", "program.c", 0, LOG_LEVEL_TRACE);

	if (argc < 3){
		log_error(logs, "No se envio un parametro al proceso program");
		return 0;
	}

	socket_kernel = conectarCliente(ip, port, logs);

	if (socket_kernel < 0){
		log_error(logs, "El cliente no se pudo conectar correctamente");
		log_destroy(logs);
		config_destroy(config);
		return 0;
	}

	log_info(logs, "El proceso se conecto correctamente con el kernel");

	enviarProgramaAlKernel(argv[2]);

	recibirSentencia();

	//ya recibio la sentencia, falta imprimirla por consola

	log_destroy(logs);
	cerrarSocket(socket_kernel);
	return 0;
}
