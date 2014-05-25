/*
 * functions.c
 *
 *  Created on: 24/05/2014
 *      Author: utnso
 */

#include "functions.h"

void enviarProgramaAlKernel(char* programa){
	tam->length = strlen(programa) + 1;
	if (enviarDatos(socket_kernel, tam, programa, logs))
		log_info(logs, "El script para procesar fue enviado");
	else
		log_error(logs, "Se produjo un error enviando el script para procesar");
}

void recibirSentencia(){
	if (recibirDatos(socket_kernel, tam, (void*)&sentencia, logs))
		log_info(logs, "La sentencia fue recibida correctamente");
	else
		log_error(logs, "Se produjo un error recibiendo la sentencia");
}
