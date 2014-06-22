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

/* Funcion que recibe el tamanio del stack seteado en archivo de configuracion del kernel */
int recibirTamanioStack(){
	int tamanio;
	if(!recibirDatos(socket_kernel, tam, (void*)&tamanio, logs)){
		log_error(logs, "Se produjo un error al recibir el quantum del kernel");
		tamanio = -1;
	}
	return tamanio;
}

/* Funcion que carga el diccionario de variables para el contexto de ejecucion actual */
void cargarDiccionario(){
	t_puntero posicion = 0;
	t_nombre_variable variable;
	int i;
	for (i=0; i < pcb->tamanio_contexto; i++){
		memcpy(&variable, stack + posicion, 1);
		char var[2];
		var[0] = variable;
		var[1] = '\0';
		dictionary_put(diccionarioDeVariables, var, &posicion);
		posicion += 5;
	}
	log_info(logs, "Se cargo el diccionario de variables");
}

/* Funcion que le pide el stack a la UMV */
void pedirStack(int tamanio){
	t_etiqueta* et = malloc(sizeof(t_etiqueta));
	et->base = pcb->segmento_stack;
	et->offset = 0;
	et->tamanio = tamanio;
	tam->length = sizeof(t_etiqueta);
	tam->menu = LEER_SEGMENTO;

	if(!enviarDatos(socket_umv, tam, et, logs))
		log_error(logs, "Se produjo un error enviando la base a la UMV");

	if(!recibirDatos(socket_umv, tam, (void*)&stack, logs))
		log_error(logs, "Se produjo un error recibiendo el segmento");
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

/* Funcion que recibe la sentencia de la UMV */
char* recibirSentencia(){
	t_etiqueta* et = malloc(sizeof(t_etiqueta));

	et->base = pcb->indice_codigo;
	et->offset = pcb->program_counter * 8;
	et->tamanio = 8;
	tam->menu = PEDIR_SENTENCIA;
	tam->length = sizeof(t_etiqueta);

	if (!enviarDatos(socket_umv, tam, et, logs))
		log_error(logs, "Se produjo un error al enviar el indice de codigo.");

	if (!recibirDatos(socket_umv, tam, (void*)et, logs))
		log_error(logs, "Se produjo un error al recibir el segmento de codigo");

	et->base = pcb->segmento_codigo;

	if (!enviarDatos(socket_umv, tam, et, logs))
		log_error(logs, "Se produjo un error al enviar el segmento de codigo");

	char* sentencia;
	if (!recibirDatos(socket_umv, tam, (void*)&sentencia, logs))
		log_error(logs, "Se produjo un error al recibir la sentencia.");

	log_info(logs, "Sentencia recibida: %s", &sentencia);
	return string_from_format("%s", &sentencia);
}

/* Funcion que depura la sentencia eliminando los \n finales */
char* depurarSentencia(char* sentencia){
	int i = strlen(sentencia);
	while (string_ends_with(sentencia, "\n")){
		i--;
		sentencia = string_substring_until(sentencia, i);
	}
	string_append(&sentencia, "\0");
	log_info(logs, "Sentencia depurada: %s", sentencia);
	return sentencia;
}

/* Funcion que libera las estructuras usadas */
void liberarEstructuras(){
	free(pcb);
	free(tam);
	dictionary_destroy(diccionarioDeVariables);
	config_destroy(config);
	log_destroy(logs);
}

/* Funcion que se lanza en el llamado de la senial SIGUSR1 */
void manejarSenial(){
	seguir = 0;
	log_debug(logs, "Se llamo a la senial SIGUSR1");
}
