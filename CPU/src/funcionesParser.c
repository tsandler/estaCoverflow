/*
 * funcionesParser.c
 *
 *  Created on: 18/05/2014
 *      Author: utnso
 */

#include "funcionesParser.h"
#include "globales.h"

t_puntero definirVariable(t_nombre_variable identificador_variable){
	tam->menu = DEFINIR_VARIABLE;
	tam->length = sizeof(identificador_variable);
	t_variable variable;
	if (!enviarDatos(socket_umv, tam, &identificador_variable, logs))
		log_error(logs, "No se pudieron enviar los datos para definir variable a la UMV");
	if (!recibirDatos(socket_umv, tam, (void*)&variable, logs))
		log_error(logs, "Se produjo un error al recibir la posicion de una variable luego de definirla");
	dictionary_put(diccionarioDeVariables, &identificador_variable, &variable);
	return variable.posicion;
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable){
	tam->menu = OBTENER_POSICION_VARIABLE;
	tam->length = sizeof(identificador_variable);
	t_puntero posicion;
	if (!enviarDatos(socket_umv, tam, &identificador_variable, logs))
		log_error(logs, "Se produjo un error enviando los datos para obtener la posicion de la variable");
	if (!recibirDatos(socket_umv, tam, (void*)&posicion, logs)){
		log_error(logs, "Se produjo un error recibiendo la posicion de una variable");
		return -1;
	}
	return posicion;
}

t_valor_variable dereferenciar(t_puntero direccion_variable){
	tam->menu = DEREFERENCIAR;
	tam->length = sizeof(direccion_variable);
	t_valor_variable posicion;
	if (!enviarDatos(socket_umv, tam, &direccion_variable, logs))
		log_error(logs, "Se produjo un error enviando los datos para dereferenciar la variable");
	if (!recibirDatos(socket_umv, tam, (void*)&posicion, logs))
		log_error(logs, "Se produjo un error recibiendo la posicion de una variable");
	return posicion;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor ){
	tam->menu = ASIGNAR;
	t_val_variable variable;
	variable.direccion_variable = direccion_variable;
	variable.valor = valor;
	tam->length = sizeof(variable);
	if (!enviarDatos(socket_umv, tam, &variable, logs))
		log_error(logs, "Se produjo un error enviando los datos para asignar un valor a una variable");
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable){
	tam->menu = OBTENER_VALOR_COMPARTIDA;
	tam->length = sizeof(variable);
	t_valor_variable valorVariable;
	if (!enviarDatos(socket_kernel, tam, variable, logs))
		log_error(logs, "Se produjo un error enviando la variable para obtener su valor");
	if (!recibirDatos(socket_kernel, tam, (void*)&valorVariable, logs))
		log_error(logs, "Se produjo un error recibiendo el valor de la variable");
	return valorVariable;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	tam->menu = ASIGNAR_VALOR_COMPARTIDA;
	t_variable_asignar var;
	var.valor = valor;
	var.variable = variable;
	tam->length = sizeof(var);
	if (!enviarDatos(socket_kernel, tam, &var, logs))
		log_error(logs, "Se produjo un error enviando la variable y su valor para asignarselo");
	return valor;
}

void irAlLabel(t_nombre_etiqueta etiqueta){
	tam->menu = IR_AL_LABEL;
	tam->length = sizeof(etiqueta);
	if (!enviarDatos(socket_kernel, tam, &etiqueta, logs)) //kernel o umv?
			log_error(logs, "Se produjo un error enviando la etiqueta");
}

void imprimir(t_valor_variable valor_mostrar){
	tam->menu = IMPRIMIR;
	tam->length = sizeof(valor_mostrar);
	if (!enviarDatos(socket_kernel, tam, &valor_mostrar, logs))
		log_error(logs, "Se produjo un error enviando el valor para imprimir");
}

void imprimirTexto(char* texto){
	tam->menu = IMPRIMIR_TEXTO;
	tam->length = strlen(texto) + 1;
	if (!enviarDatos(socket_kernel, tam, texto, logs))
		log_error(logs, "Se produjo un error enviando el texto para imprimir");
}

void wait(t_nombre_semaforo identificador_semaforo){
	tam->menu = WAIT;
	tam->length = sizeof(identificador_semaforo);
	if (!enviarDatos(socket_kernel, tam, &identificador_semaforo, logs))
		log_error(logs, "Se produjo un error enviando la senial de wait a un semaforo");
}

void signal(t_nombre_semaforo identificador_semaforo){
	tam->menu = SIGNAL;
	tam->length = sizeof(identificador_semaforo);
	if (!enviarDatos(socket_kernel, tam, &identificador_semaforo, logs))
		log_error(logs, "Se produjo un error enviando la senial de signal a un semaforo");
}

