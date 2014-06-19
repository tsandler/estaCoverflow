/*
 * manejoDelPCB.c
 *
 *  Created on: 24/05/2014
 *      Author: utnso
 */


#include "manejoDelPCB.h"
#include "../parser/metadata_program.h"


int identificadorUnico = 0;


int algoritmoDePeso(int cantEtq, int cantfc, t_size cantInstruc){

	int peso = 5*cantEtq+ 3*cantfc + cantInstruc;
	return peso;
}


registroPCB* armarPCB(char* program, int fd){


	t_metadata_program* metadataP = metadata_desde_literal(program);

	int peso =algoritmoDePeso(metadataP->cantidad_de_etiquetas,metadataP->cantidad_de_funciones,metadataP->instrucciones_size);

	registroPCB* unPCB;

	unPCB=malloc(sizeof(int)*9);

	unPCB->peso = peso;
	identificadorUnico =+ 1;
    unPCB->pid = identificadorUnico;

	return unPCB;


}


