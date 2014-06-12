/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdlib.h>
#include "globales.h"
#include "functions.h"
#include "funcionesParser.h"

int main(int argc, char** argv){

	logs = log_create("log", "CPU.c", 0, LOG_LEVEL_TRACE);

	if (argc < 2){
		log_error(logs, "No se pasaron parametros.");
		log_destroy(logs);
		return 0;
	}

	config = config_create(argv[1]);
	diccionarioDeVariables = dictionary_create();
	tam = malloc(sizeof(t_length));

	if (!archivoDeConfiguracionValido()){
		log_error(logs, "El archivo de configuracion no tiene todos los campos necesarios");
		liberarEstructuras();
		return 0;
	}

	socket_umv = conectarUMV();
	socket_kernel = conectarKernel();
	int quantum = recibirQuantum();

	if(socket_kernel < 0 || socket_umv < 0 || quantum < 0){ //Si no se conecto al kernel o UMV no puede continuar y termina la ejecucion
		log_error(logs, "El programa tuvo que finalizar insatisfactoriamente");
		liberarEstructuras();
		return 0;
	}

	inicializarFuncionesParser();
	int cont;
	char* sentencia;

	while (1){
		desplazamiento = 0; //FIXME: ver cursor de stack en la PCB (stack pointer)
		cont = 0;
		if(!recibirDatos(socket_kernel, tam, (void*)&pcb, logs))
			log_error(logs, "Se produjo un error al recibir el PCB del kernel");

		tam->length = sizeof(pcb->puntero_etiquetas_funciones);
		tam->menu = LEER_SEGMENTO;

		if(!enviarDatos(socket_umv, tam, &pcb->puntero_etiquetas_funciones, logs))
			log_error(logs, "Se produjo un error enviando la base a la UMV");

		if(!recibirDatos(socket_umv, tam, (void*)&stack, logs))
			log_error(logs, "Se produjo un error recibiendo el segmento");

		systemCall = false;

		while (quantum > cont && !systemCall){

			pcb->program_counter++; //FIXME: no aumenta siempre
			tam->length = sizeof(registroPCB);
			tam->menu = PEDIR_SENTENCIA;
			if (!enviarDatos(socket_umv, tam, pcb, logs))//FIXME: enviar base, offset y tamanio
				log_error(logs, "Se produjo un error al enviar el indice de codigo.");

			if (!recibirDatos(socket_umv, tam, (void*)&sentencia, logs))
				log_error(logs, "Se produjo un error al recibir la sentencia.");

			analizadorLinea(sentencia, &functions, &kernel_functions);

			cont++;
		}

		if(tam->menu != FINALIZAR)//TODO: tiene que salir del while o se queda esperando?
			tam->menu = CONCLUYO_UN_QUANTUM;

		tam->length = sizeof(registroPCB);

		if (!enviarDatos(socket_kernel, tam, pcb, logs))
			log_error(logs, "Se produjo un error al notificar al pcp que concluyo un quantum.");

		tam->menu = RETORNO_DE_STACK;
		tam->length = sizeof(stack);
		if(!enviarDatos(socket_umv, tam, stack, logs))
			log_error(logs, "Se produjo un error al devolverle el stack a la umv");
	}

	cerrarSocket(socket_kernel);
	cerrarSocket(socket_umv);
	liberarEstructuras();

	return 0;
}
