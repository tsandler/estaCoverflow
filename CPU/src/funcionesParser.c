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
		log_error(logs, "No se pudieron enviar los datos para definir variable %c a la UMV", identificador_variable);
	else
		log_info(logs, "La variable %c se envio correctamente", identificador_variable);

	if (!recibirDatos(socket_umv, tam, (void*)&variable, logs))
		log_error(logs, "Se produjo un error al recibir la posicion de una variable luego de definirla");
	else
		log_info(logs, "La posicion de la variable se recibio correctamente");

	dictionary_put(diccionarioDeVariables, &identificador_variable, &variable);
	return variable.posicion;
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable){
	tam->menu = OBTENER_POSICION_VARIABLE;
	tam->length = sizeof(identificador_variable);
	t_puntero posicion;

	if (!enviarDatos(socket_umv, tam, &identificador_variable, logs))
		log_error(logs, "Se produjo un error enviando los datos para obtener la posicion de la variable %c", identificador_variable);
	else
		log_info(logs, "El identificador de la variable %c para obtener su posicion se envio correctamente", identificador_variable);

	if (!recibirDatos(socket_umv, tam, (void*)&posicion, logs)){
		log_error(logs, "Se produjo un error recibiendo la posicion de una variable");
		return -1;
	}
	log_info(logs, "La posicion de la variable se recibio correctamente");
	return posicion;
}

t_valor_variable dereferenciar(t_puntero direccion_variable){
	tam->menu = DEREFERENCIAR;
	tam->length = sizeof(direccion_variable);
	t_valor_variable posicion;

	if (!enviarDatos(socket_umv, tam, &direccion_variable, logs))
		log_error(logs, "Se produjo un error enviando los datos para dereferenciar la variable %d", direccion_variable);
	else
		log_info(logs, "Se enviaron los datos para referenciar la variable %d", direccion_variable);

	if (!recibirDatos(socket_umv, tam, (void*)&posicion, logs))
		log_error(logs, "Se produjo un error recibiendo la posicion de una variable");
	else
		log_info(logs, "Se recibio la posicion de la variable correctamente");

	return posicion;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor ){
	tam->menu = ASIGNAR;
	t_val_variable variable;
	variable.direccion_variable = direccion_variable;
	variable.valor = valor;
	tam->length = sizeof(variable);

	if (!enviarDatos(socket_umv, tam, &variable, logs))
		log_error(logs, "Se produjo un error enviando los datos para asignar el valor %d a la variable %d", valor, direccion_variable);
	else
		log_info(logs, "Se enviaron los datos para asignar el valor %d a la variable %d", valor, direccion_variable);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable){
	tam->menu = OBTENER_VALOR_COMPARTIDA;
	tam->length = sizeof(variable);
	t_valor_variable valorVariable;

	if (!enviarDatos(socket_kernel, tam, variable, logs))
		log_error(logs, "Se produjo un error enviando la variable %s para obtener su valor", variable);
	else
		log_info(logs, "Se envio la variable %s para obtener su valor", variable);

	if (!recibirDatos(socket_kernel, tam, (void*)&valorVariable, logs))
		log_error(logs, "Se produjo un error recibiendo el valor de la variable");
	else
		log_info(logs, "Se recibio el valor de la variable correctamente");

	return valorVariable;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	tam->menu = ASIGNAR_VALOR_COMPARTIDA;
	t_variable_asignar var;
	var.valor = valor;
	var.variable = variable;
	tam->length = sizeof(var);

	if (!enviarDatos(socket_kernel, tam, &var, logs))
		log_error(logs, "Se produjo un error enviando la variable %s y el valor %d para asignarselo", variable, valor);
	else
		log_info(logs, "Se envio la variable %s para asignarle el valor %d", variable, valor);
	return valor;
}

void irAlLabel(t_nombre_etiqueta etiqueta){
	tam->menu = IR_AL_LABEL;
	tam->length = sizeof(etiqueta);

	if (!enviarDatos(socket_kernel, tam, &etiqueta, logs)) //kernel o umv?
		log_error(logs, "Se produjo un error enviando la etiqueta %s", etiqueta);
	else
		log_info(logs, "Se envio la etiqueta %s para ir al label", etiqueta);
}

void llamarSinRetorno(t_nombre_etiqueta etiqueta){
	tam->menu = LLAMAR_SIN_RETORNO;

}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar){
	tam->menu = LLAMAR_CON_RETORNO;
}

void finalizar(){
	tam->menu = FINALIZAR;
}

void retornar(t_valor_variable retorno){
	tam->menu = RETORNAR;
}

void imprimir(t_valor_variable valor_mostrar){
	tam->menu = IMPRIMIR;
	tam->length = sizeof(valor_mostrar);

	if (!enviarDatos(socket_kernel, tam, &valor_mostrar, logs))
		log_error(logs, "Se produjo un error enviando el valor %d para imprimir", valor_mostrar);
	else
		log_info(logs, "Se envio el valor %d para imprimirlo", valor_mostrar);
}

void imprimirTexto(char* texto){
	tam->menu = IMPRIMIR_TEXTO;
	tam->length = strlen(texto) + 1;

	if (!enviarDatos(socket_kernel, tam, texto, logs))
		log_error(logs, "Se produjo un error enviando el texto %s para imprimirlo", texto);
	else
		log_info(logs, "Se envio el texto %s para imprimirlo", texto);
}

void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo){
	tam->menu = ENTRADA_SALIDA;
}

void wait(t_nombre_semaforo identificador_semaforo){
	tam->menu = WAIT;
	tam->length = sizeof(identificador_semaforo);
	if (!enviarDatos(socket_kernel, tam, &identificador_semaforo, logs))
		log_error(logs, "Se produjo un error enviando la senial de wait al semaforo %s", identificador_semaforo);
	else
		log_info(logs, "Se envio correctamente la senial de wait al semaforo %s", identificador_semaforo);
}

void signal(t_nombre_semaforo identificador_semaforo){
	tam->menu = SIGNAL;
	tam->length = sizeof(identificador_semaforo);

	if (!enviarDatos(socket_kernel, tam, &identificador_semaforo, logs))
		log_error(logs, "Se produjo un error enviando la senial de signal al semaforo %s", identificador_semaforo);
	else
		log_info(logs, "Se envio correctamente la senial de signal al semaforo %s", identificador_semaforo);
}

