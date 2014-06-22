/*
 * functions.c
 *
 *  Created on: 24/05/2014
 *      Author: utnso
 */

#include "functions.h"

static char* _obtenerPrograma(char* programa);

/* Funcion que envia el programa a ejecutar al kernel */
void enviarProgramaAlKernel(char* path){
	char* programa = _obtenerPrograma(path);
	tam->menu = ENVIO_SCRIPT;
	tam->length = strlen(programa) + 1;
	if (enviarDatos(socket_kernel, tam, programa, logs))
		log_info(logs, "El script para procesar fue enviado");
	else
		log_error(logs, "Se produjo un error enviando el script para procesar");
}

/* Funcion que recibe el programa desde un path indicado */
char* _obtenerPrograma(char* path){
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

/* Funcion que recibe la sentencia a imprimir */
char* recibirSentencia(){
	char* sent;
	if (recibirDatos(socket_kernel, tam, (void*)&sent, logs))
		log_info(logs, "La sentencia fue recibida correctamente");
	else
		log_error(logs, "Se produjo un error recibiendo la sentencia");

	return string_from_format("%s", &sent);
}

/* Funcion que se conecta al kernel y devuelve el socket al que conecto */
int conectarKernel(){
	char *ip = config_get_string_value(config, "IP");
	int port = config_get_int_value(config, "PUERTO_KERNEL");
	return conectarCliente(ip, port, logs);
}

/* Funcion para inicializar las variables globales */
void inicializarVariables(){
	char* path = getenv("ANSISOP_CONFIG");
	config = config_create(path);
	tam = malloc(sizeof(t_length));
	logs = log_create("log", "program.c", 1, LOG_LEVEL_TRACE);
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
	config_destroy(config);
	log_destroy(logs);
}
