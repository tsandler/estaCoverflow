/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "globales.h"
#include "functions.h"
#include "funcionesParser.h"

int main(int argc, char** argv){

	logs = log_create("log", "CPU.c", 1, LOG_LEVEL_TRACE);

	if (argc < 2){
		log_error(logs, "No se pasaron parametros.");
		log_destroy(logs);
		return 0;
	}

	config = config_create(argv[1]);
	diccionarioDeVariables = dictionary_create();
	tam = malloc(sizeof(t_length));

	if (!archivo_de_configuracion_valido()){
		log_error(logs, "El archivo de configuracion no tiene todos los campos necesarios");
		liberar_estructuras();
		return 0;
	}

	socketUMV = conectar_UMV();
	socketKernel = conectar_kernel();
	int quantum = recibir_quantum();
	int tamanioStack = recibir_tamanio_stack();
	int retardo = recibir_retardo();

	if(socketKernel < 0 || socketUMV < 0 || quantum < 0 || tamanioStack < 0){
		log_error(logs, "El programa tuvo que finalizar insatisfactoriamente");
		liberar_estructuras();
		return 0;
	}
	tam->menu = SOY_CPU;
	enviarMenu(socketKernel, tam, logs);
	enviarMenu(socketUMV, tam, logs);

	inicializar_funciones_parser();

	int pc;
	seguir = 1;

	signal(SIGUSR1, manejar_senial);

	while (seguir){
		int cont = 0;
		ejecutando = 0;
		if(!recibirDatos(socketKernel, tam, (void*)pcb, logs)){
			log_error(logs, "Se produjo un error al recibir el PCB del kernel");
			break;
		}
		tam->menu = PID_ACTUAL;
		tam->length = sizeof(pcb->pid);
		if(!enviarDatos(socketUMV, tam, &pcb->pid, logs))
			log_error(logs, "Se produjo un error enviando el pid a la UMV");

		pedir_stack(tamanioStack);
		cargar_diccionario();

		systemCall = false;
		ejecutando = 1;
		while (quantum > cont && !systemCall){
			pc = pcb->program_counter;

			char* sentencia = recibir_sentencia();

			analizadorLinea(strdup(sentencia), &functions, &kernel_functions);

			if (pc == pcb->program_counter)
				pcb->program_counter++;
			cont++;
		}
		if (tam->menu != ENTRADA_SALIDA){
			if (tam->menu != FINALIZAR) //finalizo el programa
				tam->menu = CONCLUYO_UN_QUANTUM;
			tam->length = sizeof(registroPCB);
			if (!enviarDatos(socketKernel, tam, pcb, logs))
				log_error(logs, "Se produjo un error al notificar al pcp que concluyo un quantum.");
		}

		tam->menu = ESCRIBIR_SEGMENTO;
		tam->length = tamanioStack;

		if(!enviarDatos(socketUMV, tam, stack, logs))
			log_error(logs, "Se produjo un error al devolverle el stack a la umv");

		if (signalCall){
			cerrarSocket(socketKernel);
			cerrarSocket(socketUMV);
		}
		dictionary_clean(diccionarioDeVariables);
	}

	liberar_estructuras();

	return 0;
}
