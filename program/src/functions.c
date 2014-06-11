/*
 * functions.c
 *
 *  Created on: 24/05/2014
 *      Author: utnso
 */

#include "functions.h"

/* Funcion que envia el programa a ejecutar al kernel */
void enviarProgramaAlKernel(char* programa){
	tam->length = strlen(programa) + 1;
	if (enviarDatos(socket_kernel, tam, programa, logs))
		log_info(logs, "El script para procesar fue enviado");
	else
		log_error(logs, "Se produjo un error enviando el script para procesar");
}

/* Funcion que recibe la sentencia a imprimir */
void recibirSentencia(){
	if (recibirDatos(socket_kernel, tam, (void*)&sentencia, logs))
		log_info(logs, "La sentencia fue recibida correctamente");
	else
		log_error(logs, "Se produjo un error recibiendo la sentencia");
}

/* FUncion que recibe el programa desde un path indicado */
char* obtenerPrograma(char* path){
	struct stat stat_file;
	stat(path, &stat_file);
	FILE* file = NULL;
	char* buffer;
	file = fopen(path, "r");
	if (file != NULL) {
		buffer = calloc(1, stat_file.st_size + 1);
		fread(buffer, stat_file.st_size, 1, file);
	}
	fclose(file);
	return buffer;
}

/* Funcion que se conecta al kernel y devuelve el socket al que conecto */
int conectarKernel(){
	char *ip = config_get_string_value(config, "IP");
	int port = config_get_int_value(config, "PUERTO_KERNEL");
	int socket = conectarCliente(ip, port, logs);
	if (socket < 0)
		log_error(logs, "El cliente no se pudo conectar correctamente");

	return socket;
}

/* Funcion para inicializar las variables globales */
void inicializarVariables(){
	path = getenv("ANSISOP_CONFIG");
	config = config_create(path);
	logs = log_create("log", "program.c", 0, LOG_LEVEL_TRACE);
}

/* Funcion para verificar si el archivo de configuracion es valido */
int archivoDeConfiguracionValido(){
	if (!config_has_property(config, "IP"))
		return 0;
	if (!config_has_property(config, "PUERTO_KERNEL"))
		return 0;
	return 1;
}

/* Funcion para liberar las estructuras usadas */
void liberarEstructuras(){
	free(path);
	config_destroy(config);
	log_destroy(logs);
}
