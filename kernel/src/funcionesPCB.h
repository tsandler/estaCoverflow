/*
 * manejoDelPCB.h
 *
 *  Created on: 24/05/2014
 *      Author: utnso
 */

#ifndef MANEJODELPCB_H_
#define MANEJODELPCB_H_

#include <stdio.h>
#include <stdlib.h>
#include "../parser/metadata_program.h"
#include "sockets.h"
#include "../commons/log.h"
#include "../commons/config.h"



typedef struct registroPCB{
    int pid;

	int segmento_stack;
	int cursor_stack;
	int tamanio_contexto;

	int segmento_codigo;
	int tamanio_indice_codigo ;
	int indice_codigo;
	int program_counter;

	int indice_etiquetas_funciones;
	int puntero_etiquetas_funciones;
	int tamanio_indice_etiquetas_funciones;

	int retrasoIO;
	int peso;
	int fd;

}registroPCB;

registroPCB* armarPCB(char* program, int fd);
int algoritmoDePeso(int cantEtq, int cantfc, t_size cantInstruc);
void eliminarSegmentoUMV(int socket_UMV, t_log* logs,registroPCB* PCBprograma);
void conectarseUMV();
void intercambiarDatosUMV(int socket_UMV, t_log* logs, registroPCB* PCBprograma, char* buf);

//FIXME: agregar cabeceras de funciones

#endif /* MANEJODELPCB_H_ */
