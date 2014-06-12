/*
 * functions.c
 *
 *  Created on: 03/06/2014
 *      Author: utnso
 */

#include "functions.h"

/* Funcion que conecta a la CPU con la UMV */
int conectarUMV(){
	char *ip = config_get_string_value(config, "IP_UMV");
	int port = config_get_int_value(config, "PUERTO_UMV");
	int socket_umv = conectarCliente(ip, port, logs);
	if(socket_umv < 0 ){
		log_error(logs, "No se pudo conectar a la UMV correctamente");
		socket_umv = -1;
	}else
		log_info(logs, "Se conecto con la UMV correctamente");
	return socket_umv;
}

/* Funcion que conecta a la CPU con el kernel */
int conectarKernel(){
	char *ip = config_get_string_value(config, "IP_KERNEL");
	int port = config_get_int_value(config, "PUERTO_KERNEL");
	int socket_kernel = conectarCliente(ip, port, logs);
	if(socket_kernel < 0 ){
		log_error(logs, "No se pudo conectar a la kernel correctamente");
		socket_kernel = -1;
	}else
		log_info(logs, "Se conecto con el kernel correctamente");
	return socket_kernel;
}

/* Funcion que recibe el quantum enviado por el kernel */
int recibirQuantum(){
	int quantum;
	if(!recibirDatos(socket_kernel, tam, (void*)&quantum, logs)){
		log_error(logs, "Se produjo un error al recibir el quantum del kernel");
		quantum = -1;
	}
	return quantum;
}

int obtenerPosicion(char variable){
	int x;
	for (x=0;x<sizeof(stack); x++){
		if (stack[x] == variable)
			break;
	}
	return x;
}

/* Funcion que verifica que sea un archivo de configuracion valido */
int archivoDeConfiguracionValido(){
	if (!config_has_property(config, "IP_KERNEL"))
		return 0;
	if (!config_has_property(config, "IP_UMV"))
		return 0;
	if (!config_has_property(config, "PUERTO_KERNEL"))
		return 0;
	if (!config_has_property(config, "PUERTO_UMV"))
		return 0;
	return 1;
}

void inicializarEstructuras(){
	logs = log_create("log", "CPU.c", 0, LOG_LEVEL_TRACE);
	diccionarioDeVariables = dictionary_create();
	tam = malloc(sizeof(t_length));
}

/* Funcion que libera las estructuras usadas */
void liberarEstructuras(){
	free(pcb);
	free(tam);
	dictionary_destroy(diccionarioDeVariables);
	config_destroy(config);
	log_destroy(logs);
}
