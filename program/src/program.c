/*
 ============================================================================
 Name        : program.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "globales.h"
#include "functions.h"
#include <commons/string.h>
#include <string.h>

int main(int argc, char **argv) {

	inicializar_variables();

	if (!archivo_de_configuracion_valido()){
		log_error(logs, "El archivo de configuracion no tiene todos los campos necesarios");
		liberar_estructuras();
		return 0;
	}

	socketKernel = conectar_kernel();
	if (socketKernel < 0){
		log_error(logs, "El programa tuvo que finalizar insatisfactoriamente porque no se conecto al kernel");
		liberar_estructuras();
		return 0;
	}
	log_info(logs, "El proceso se conecto correctamente con el kernel");

	enviar_programa_al_kernel(argv[1]);

	while (tam->menu != FINALIZAR){
		char* sentencia = recibir_sentencia();
		printf("%s/n", sentencia);
	}

	cerrarSocket(socketKernel);
	liberar_estructuras();

	return 0;
}
