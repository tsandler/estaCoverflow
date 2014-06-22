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

	if (!archivoDeConfiguracionValido()){
		log_error(logs, "El archivo de configuracion no tiene todos los campos necesarios");
		liberarEstructuras();
		return 0;
	}

	socket_umv = conectarUMV();
	socket_kernel = conectarKernel();
	int quantum = recibirQuantum();
	int tamanioStack = recibirTamanioStack();

	if(socket_kernel < 0 || socket_umv < 0 || quantum < 0 || tamanioStack < 0){ //Si no se conecto al kernel o UMV no puede continuar y termina la ejecucion
		log_error(logs, "El programa tuvo que finalizar insatisfactoriamente");
		liberarEstructuras();
		return 0;
	}
	tam->menu = SOY_CPU;
	enviarMenu(socket_kernel, tam, logs);
	enviarMenu(socket_umv, tam, logs);

	inicializarFuncionesParser();

	int pc;
	seguir = 1;

	signal(SIGUSR1, manejarSenial);

	while (seguir){
		int cont = 0;
		if(!recibirDatos(socket_kernel, tam, (void*)pcb, logs))
			log_error(logs, "Se produjo un error al recibir el PCB del kernel");

		tam->menu = PID_ACTUAL;
		tam->length = sizeof(pcb->pid);
		if(!enviarDatos(socket_umv, tam, &pcb->pid, logs))
			log_error(logs, "Se produjo un error enviando el pid a la UMV");

		pedirStack(tamanioStack);
		cargarDiccionario();

		systemCall = false;

		while (quantum > cont && !systemCall && seguir){
			pc = pcb->program_counter;

			char* sentencia = recibirSentencia();

			analizadorLinea(strdup(sentencia), &functions, &kernel_functions);

			cont++;

			if (pc == pcb->program_counter)
				pcb->program_counter++;
		}

		tam->menu = CONCLUYO_UN_QUANTUM;
		tam->length = sizeof(registroPCB);

		if (!enviarDatos(socket_kernel, tam, pcb, logs))
			log_error(logs, "Se produjo un error al notificar al pcp que concluyo un quantum.");

		tam->menu = ESCRIBIR_SEGMENTO;
		tam->length = tamanioStack;

		if(!enviarDatos(socket_umv, tam, stack, logs))
			log_error(logs, "Se produjo un error al devolverle el stack a la umv");

		dictionary_clean(diccionarioDeVariables);
	}

	cerrarSocket(socket_kernel);
	cerrarSocket(socket_umv);
	liberarEstructuras();

	return 0;
}
