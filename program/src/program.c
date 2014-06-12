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

int main(int argc, char **argv) {

	inicializarVariables();

/*	if (!archivoDeConfiguracionValido()){
		log_error(logs, "El archivo de configuracion no tiene todos los campos necesarios");
		liberarEstructuras();
		return 0;
	}*/


	socket_kernel = conectarKernel();
	if (socket_kernel < 0){
		log_error(logs, "El programa tuvo que finalizar insatisfactoriamente porque no se conecto al kernel");
		liberarEstructuras();
		return 0;
	}
	log_info(logs, "El proceso se conecto correctamente con el kernel");

	char* programa = obtenerPrograma("facil.ansisop");
	enviarProgramaAlKernel(programa);

	while (tam->menu != FINALIZAR){
		recibirSentencia();
		printf("%s/n", sentencia);
	}

	cerrarSocket(socket_kernel);
	liberarEstructuras();

	return 0;
}
