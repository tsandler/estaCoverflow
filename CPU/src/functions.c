/*
 * functions.c
 *
 *  Created on: 03/06/2014
 *      Author: utnso
 */

#include "functions.h"

static char* _depurar_sentencia(char* sentencia, int tamanio);

typedef struct{
	int offset;
	int tamanio;
}t_aux;

/* Funcion que conecta a la CPU con la UMV */
int conectar_UMV(){
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
int conectar_kernel(){
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

/* Funcion que recibe el quantum, el tamanio del stack o el retardo */
int recibir(int dato){
	int* valor;
	switch(dato){
		case 1:
			if(!recibirDatos(socketKernel, tam, (void*)&valor, logs)){
				log_error(logs, "Se produjo un error al recibir el quantum del kernel");
				*valor = -1;
			}else
				log_info(logs, "Se recibio el quantum de %d", *valor);
			break;
		case 2:
			if(!recibirDatos(socketKernel, tam, (void*)&valor, logs)){
				log_error(logs, "Se produjo un error al recibir el quantum del kernel");
				*valor = -1;
			}else
				log_info(logs, "Se recibio el retardo %d", *valor);
			break;

	}
	return *valor;
}

/* Funcion que carga el diccionario de variables para el contexto de ejecucion actual */
void cargar_diccionario(){
	t_nombre_variable variable;
	int inicial;
	if (pcb->tamanio_contexto > 0){
		if (pcb->cursor_stack != pcb->cursor_anterior)
			inicial = pcb->cursor_stack - 5 * pcb->tamanio_contexto;
		else{
			inicial = pcb->cursor_stack;
			pcb->cursor_stack += 5 * pcb->tamanio_contexto;
		}
		int i, cont = 0;
		t_etiqueta* etiq = malloc(sizeof(t_etiqueta));
		etiq->base = pcb->segmento_stack;
		etiq->offset = inicial;
		etiq->tamanio = pcb->tamanio_contexto * 5;
		tam->menu = LEER_SEGMENTO;
		tam->length = sizeof(t_etiqueta);
		enviarDatos(socketUMV, tam, etiq, logs);

		unsigned char* stack;
		recibirDatos(socketUMV, tam, (void*)&stack, logs);

		for (i=0; i < pcb->tamanio_contexto; i++){
			int aux = inicial + i*5;
			int* pos = malloc(sizeof(int));
			memcpy(pos, &aux, 4);
			memcpy(&variable, stack + (i*5), 1);
			char var[2];
			var[0] = variable;
			var[1] = '\0';
			dictionary_put(diccionarioDeVariables, var, pos);
			cont++;
		}
		free(stack);
	}
	log_info(logs, "Se cargo el diccionario de variables");
}

/* Funcion que verifica que sea un archivo de configuracion valido */
int archivo_de_configuracion_valido(){
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

void vaciarDiccionario(){
	if (!dictionary_is_empty(diccionarioDeVariables))
		dictionary_clean(diccionarioDeVariables);
}

/* Funcion que recibe la sentencia de la UMV */
char* recibir_sentencia(){
	t_aux* aux;
	t_etiqueta* et = malloc(sizeof(t_etiqueta));
	et->base = pcb->indice_codigo;
	et->offset = pcb->program_counter * 8;
	et->tamanio = 8;
	tam->menu = PEDIR_SENTENCIA;
	tam->length = sizeof(t_etiqueta);

	if (!enviarDatos(socketUMV, tam, et, logs))
		log_error(logs, "Se produjo un error al enviar el indice de codigo.");

	if (!recibirDatos(socketUMV, tam, (void*)&aux, logs))
		log_error(logs, "Se produjo un error al recibir el segmento de codigo");
	et->base = pcb->segmento_codigo;
	et->offset = aux->offset;
	et->tamanio = aux->tamanio;
	int tamanio;
	memcpy(&tamanio, &aux->tamanio, 4);
	tam->length = sizeof(t_etiqueta);
	if (!enviarDatos(socketUMV, tam, et, logs))
		log_error(logs, "Se produjo un error al enviar el segmento de codigo");

	char* sentencia;
	if (!recibirDatos(socketUMV, tam, (void*)&sentencia, logs))
		log_error(logs, "Se produjo un error al recibir la sentencia.");

	return _depurar_sentencia(sentencia, tamanio);
}

/* Funcion que depura la sentencia eliminando los \n finales */
static char* _depurar_sentencia(char* sentencia, int tamanio){
	char* sent = string_substring_until(sentencia, tamanio);
	string_trim(&sent);
	log_info(logs, "Sentencia depurada: %s", sent);
	return sent;
}

/* Funcion que libera las estructuras usadas */
void liberar_estructuras(){
	free(tam);
	dictionary_destroy(diccionarioDeVariables);
	config_destroy(config);
	log_destroy(logs);
}

/* Funcion que se lanza en el llamado de la senial SIGUSR1 */
void manejar_senial(){
	seguir = 0;
	if (!ejecutando){
		cerrarSocket(socketKernel);
		cerrarSocket(socketUMV);
		log_info(logs, "Se llamo a la senial SIGUSR1");
		log_info(logs, "Cerrando la CPU...");
		liberar_estructuras();
		exit(0);
	}
}
