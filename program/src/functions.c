/*
 * functions.c
 *
 *  Created on: 24/05/2014
 *      Author: utnso
 */

#include "functions.h"

static char* _obtener_programa(char* programa);

/* Funcion que envia el programa a ejecutar al kernel */
void enviar_programa_al_kernel(char* path){
	char* programa = _obtener_programa(path);
	tam->menu = ENVIO_SCRIPT;
	tam->length = strlen(programa) + 1;
	if (enviarDatos(socketKernel, tam, programa, logs))
		log_info(logs, "El script para procesar fue enviado");
	else
		log_error(logs, "Se produjo un error enviando el script para procesar");
}

/* Funcion que recibe el programa desde un path indicado */
static char* _obtener_programa(char* path){
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
char* recibir_sentencia(){
	char* sent;
	int* val;
	if (recibirMenu(socketKernel, tam, logs)){
		switch(tam->menu){
			case IMPRIMIR:
				if(recibirDato(socketKernel, tam->length, (void*)&val, logs)){
					return string_from_format("%d", *val);
				}
				break;
			case IMPRIMIR_TEXTO:
				if(recibirDato(socketKernel, tam->length, (void*)&sent, logs)){
					return sent;
				}
				break;
			case OK:
				tam->menu = SIGNAL;
				enviarMenu(socketKernel, tam, logs);
				break;
			case SEG_FAULT:
				tam->menu = FINALIZAR;
				printf("Se produjo un segmentation fault");
				break;
			case MEMORY_OVERLOAD:
				tam->menu = FINALIZAR;
				printf("Se produjo un overload solicitando la creacion de los segmentos");
				break;
			case ERROR:
				tam->menu = FINALIZAR;
				printf("El programa tuvo que finalizar insatisfactoriamente");
				break;
			default:
				tam->menu = FINALIZAR;
				break;
		}
	}else{
		log_error(logs, "Se produjo un error recibiendo la sentencia");
		tam->menu = FINALIZAR;
	}
	return "ok";
}

/* Funcion que se conecta al kernel y devuelve el socket al que conecto */
int conectar_kernel(){
	char *ip = config_get_string_value(config, "IP");
	int port = config_get_int_value(config, "PUERTO_KERNEL");
	return conectarCliente(ip, port, logs);
}

/* Funcion para inicializar las variables globales */
void inicializar_variables(){
	char* path = getenv("ANSISOP_CONFIG");
	config = config_create(path);
	tam = malloc(sizeof(t_length));
	logs = log_create("log", "program.c", 1, LOG_LEVEL_TRACE);
}

/* Funcion para verificar si el archivo de configuracion es valido */
int archivo_de_configuracion_valido(){
	if (!config_has_property(config, "IP"))
		return 0;
	if (!config_has_property(config, "PUERTO_KERNEL"))
		return 0;
	return 1;
}

/* Funcion para liberar las estructuras usadas */
void liberar_estructuras(){
	config_destroy(config);
	log_destroy(logs);
}
