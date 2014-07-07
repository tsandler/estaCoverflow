/*
 * funcionesParser.c
 *
 *  Created on: 18/05/2014
 *      Author: utnso
 */

#include "funcionesParser.h"

void inicializar_funciones_parser(){
	functions.AnSISOP_definirVariable = definir_variable;
	functions.AnSISOP_obtenerPosicionVariable = obtener_posicion_variable;
	functions.AnSISOP_dereferenciar = dereferenciar;
	functions.AnSISOP_asignar = asignar;
	functions.AnSISOP_obtenerValorCompartida = obtener_valor_compartida;
	functions.AnSISOP_asignarValorCompartida = asignar_valor_compartida;
	functions.AnSISOP_irAlLabel = ir_al_label;
	functions.AnSISOP_llamarSinRetorno = llamar_sin_retorno;
	functions.AnSISOP_llamarConRetorno = llamar_con_retorno;
	functions.AnSISOP_finalizar = finalizar;
	functions.AnSISOP_retornar = retornar;
	functions.AnSISOP_imprimir = imprimir;
	functions.AnSISOP_imprimirTexto = imprimir_texto;
	functions.AnSISOP_entradaSalida = entrada_salida;

	kernel_functions.AnSISOP_wait = wait;
	kernel_functions.AnSISOP_signal = signal_parser;
}

/* Primitiva que almacena en el stack y en el diccionario de variables a una variable, dejandola sin inicializar*/
t_puntero definir_variable(t_nombre_variable identificador_variable){
	t_puntero* posicion = malloc(sizeof(t_puntero));
	posicion = pcb->cursor_stack;
	memcpy(stack + pcb->cursor_stack, &identificador_variable, 1);
	pcb->cursor_stack += 5;
	char variable[2];
	variable[0] = identificador_variable;
	variable[1] = '\0';
	dictionary_put(diccionarioDeVariables, variable, posicion);
	pcb->tamanio_contexto++;
	log_info(logs, "La variable %c fue definida correctamente", identificador_variable);
	return 0;
}

/* Primitiva que obtiene del diccionario de variables la posicion de una variable */
t_puntero obtener_posicion_variable(t_nombre_variable identificador_variable){
	t_puntero *posicion;
	char variable[2];
	variable[0] = identificador_variable;
	variable[1] = '\0';
	posicion = dictionary_get(diccionarioDeVariables, variable);
	log_info(logs, "La posicion de la variable %c es %d", identificador_variable, *posicion);
	return *posicion;
}

/* Primitiva que obtiene el valor de una direccion especifica */
t_valor_variable dereferenciar(t_puntero direccion_variable){
	t_valor_variable valor;
	memcpy(&valor, stack + direccion_variable + 1, 4);
	log_info(logs, "El valor de la direccion %d es %d", direccion_variable, valor);
	return valor;
}

/* Primitiva que asigna el valor de una variable, almacenandola en el stack */
void asignar(t_puntero direccion_variable, t_valor_variable valor ){
	memcpy(stack + direccion_variable + 1, &valor, 4);
	log_info(logs, "El valor %d fue asignado a la variable en la posicion %d", valor, direccion_variable);
}

/* Primitiva que pide al kernel el valor de una variable compartida */
t_valor_variable obtener_valor_compartida(t_nombre_compartida variable){
	tam->menu = OBTENER_VALOR_COMPARTIDA;
	tam->length = strlen(variable)+1;
	t_valor_variable valorVariable;

	if (!enviarDatos(socketKernel, tam, variable, logs))
		log_error(logs, "Se produjo un error enviando la variable %s para obtener su valor", variable);
	else
		log_info(logs, "Se envio la variable %s para obtener su valor", variable);

	if (!recibirDatos(socketKernel, tam, (void*)&valorVariable, logs))
		log_error(logs, "Se produjo un error recibiendo el valor de la variable");
	else
		log_info(logs, "Se recibio el valor de la variable correctamente");

	return valorVariable;
}

/* Primitiva que envia el valor a asignarle a una variable */
t_valor_variable asignar_valor_compartida(t_nombre_compartida variable, t_valor_variable valor){
	tam->menu = ASIGNAR_VALOR_COMPARTIDA;
	tam->length = sizeof(t_valor_variable);

	if (!enviarDatos(socketKernel, tam, &valor, logs))
		log_error(logs, "Se produjo un error enviando el valor %d para asignarselo", valor);

	tam->length = strlen(variable) + 1;
	if (!enviarDatos(socketKernel, tam, variable, logs))
		log_error(logs, "Se produjo un error enviando la variable %s", variable);
	else
		log_info(logs, "Se envio la variable %s para asignarle el valor %d", variable, valor);

	return valor;
}

/* Primitiva que cambia el program counter al correspondiente de una etiqueta dada */
void ir_al_label(t_nombre_etiqueta etiqueta){
	t_etiqueta* etiquetaAEnviar = malloc(sizeof(t_etiqueta));
	etiquetaAEnviar->base = pcb->indice_etiquetas_funciones;
	etiquetaAEnviar->offset = 0;
	etiquetaAEnviar->tamanio = pcb->tamanio_indice_etiquetas_funciones;
	tam->menu = LEER_SEGMENTO;
	tam->length = sizeof(t_etiqueta);
	char* etiquetas;
	if (!enviarDatos(socketUMV, tam, etiquetaAEnviar, logs))
		log_error(logs, "Se produjo un error enviando el struct de etiquetas");
	else
		log_info(logs, "Se envio el struct para pedir el segmento de etiquetas");

	if(!recibirDatos(socketUMV, tam, (void*)&etiquetas, logs))
		log_error(logs, "Se produjo un error recibiendo el segmento de etiquetas");
	else
		log_error(logs, "Se recibio el segmento de etiquetas");

	t_size tamanio = pcb->tamanio_indice_etiquetas_funciones;
	char* et = string_from_format("%s", &etiquetas);
	pcb->program_counter = metadata_buscar_etiqueta(etiqueta, et, tamanio);
}

/* Primitiva que se invoca en los procedimientos, cambia el contexto de ejecucion a una etiqueta dada */
void llamar_sin_retorno(t_nombre_etiqueta etiqueta){
	int desplazamiento = pcb->cursor_stack;
	memcpy(stack+desplazamiento, &pcb->cursor_stack, 4);
	desplazamiento += 4;
	memcpy(stack+desplazamiento, &pcb->program_counter+1, 4);
	desplazamiento += 4;

	pcb->cursor_stack = desplazamiento;
	log_info(logs, "Se llamo a la funcion llamarSinRetorno con la etiqueta %s", etiqueta);
	ir_al_label(etiqueta);
}

/* Primitiva que se invoca en las funciones, recibe la direccion donde retornar el valor y la etiqueta a la cual tiene que ir */
void llamar_con_retorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar){
	llamar_sin_retorno(etiqueta);
	int desplazamiento = pcb->cursor_stack;
	memcpy(stack+desplazamiento, &donde_retornar, 4);
	desplazamiento += 4;

	pcb->cursor_stack = desplazamiento;
	log_info(logs, "Se llamo a la funcion llamarConRetorno con la etiqueta %s y puntero %d", etiqueta, donde_retornar);
}

/* Primitiva que finaliza el contexto actual */
void finalizar(){
	if (pcb->tamanio_contexto * 5 < pcb->cursor_stack){
		memcpy(&pcb->program_counter, stack + pcb->cursor_stack - 4, 4);
		memcpy(&pcb->cursor_stack, stack + pcb->cursor_stack - 8, 4);
		log_info(logs, "Finalizando el contexto actual");
	}else{
		tam->menu = FINALIZAR;
		log_info(logs, "Finalizando el programa");
	}
}

/* Primitiva que finaliza el contexto actual y asigna el valor a retornar en su posicion correspondiente en el stack */
void retornar(t_valor_variable retorno){
	t_puntero* posicion = malloc(sizeof(t_puntero));
	memcpy(posicion, stack + pcb->cursor_stack - 4, 4);
	asignar(*posicion, retorno);
	pcb->cursor_stack -= 4;
	finalizar();
	log_info(logs, "Se llamo a la funcion retornar");
}

/* Primitiva que envia al kernel un valor para mostrar por consola */
void imprimir(t_valor_variable valor_mostrar){
	tam->menu = IMPRIMIR;
	tam->length = sizeof(t_valor_variable);

	if (!enviarDatos(socketKernel, tam, &valor_mostrar, logs))
		log_error(logs, "Se produjo un error enviando el valor %d para imprimir", valor_mostrar);
	else
		log_info(logs, "Se envio el valor %d para imprimirlo", valor_mostrar);
}

/* Primitiva que envia al kernel un texto para mostrar por consola */
void imprimir_texto(char* texto){
	tam->menu = IMPRIMIR_TEXTO;
	tam->length = strlen(texto) + 1;

	if (!enviarDatos(socketKernel, tam, texto, logs))
		log_error(logs, "Se produjo un error enviando el texto %s para imprimirlo", texto);
	else
		log_info(logs, "Se envio el texto %s para imprimirlo", texto);
}

/* Primitiva que le dice al kernel que fue a entrada y salida con un dispositivo por un determinado tiempo */
void entrada_salida(t_nombre_dispositivo dispositivo, int tiempo){
	tam->menu = ENTRADA_SALIDA;
	tam->length = sizeof(int);
	if (!enviarDatos(socketKernel, tam, &tiempo, logs))
		log_error(logs, "Se produjo un error enviando un tiempo de entrada y salida al kernel");
	tam->length = strlen(dispositivo) + 1;
	if (!enviarDatos(socketKernel, tam, dispositivo, logs))
		log_error(logs, "Se produjo un error enviando el nombre del dispositivo de entrada y salida al kernel");
	tam->length = sizeof(registroPCB);
	if (!enviarDatos(socketKernel, tam, pcb, logs))
		log_error(logs, "Se produjo un error al notificar al pcp que concluyo un quantum.");
	systemCall = true;
}

/* Primitiva que envia la senial wait de un semaforo al kernel */
void wait(t_nombre_semaforo identificador_semaforo){
	tam->menu = WAIT;
	tam->length = strlen(identificador_semaforo);

	if (!enviarDatos(socketKernel, tam, identificador_semaforo, logs))
		log_error(logs, "Se produjo un error enviando la senial de wait al semaforo %s", identificador_semaforo);
	else
		log_info(logs, "Se envio correctamente la senial de wait al semaforo %s", identificador_semaforo);

	if (!recibirDatos(socketKernel, tam, (void*)&systemCall, logs))
		log_error(logs, "Se produjo un error recibiendo el resultado del wait a un semaforo");
	else
		log_info(logs, "Se recibio correctamente el resultado de la senial wait del kernel");
}

/* Primitiva que envia la senial de signal de un semaforo al kernel */
void signal_parser(t_nombre_semaforo identificador_semaforo){
	tam->menu = SIGNAL;
	tam->length = strlen(identificador_semaforo);

	if (!enviarDatos(socketKernel, tam, identificador_semaforo, logs))
		log_error(logs, "Se produjo un error enviando la senial de signal al semaforo %s", identificador_semaforo);
	else
		log_info(logs, "Se envio correctamente la senial de signal al semaforo %s", identificador_semaforo);
}

