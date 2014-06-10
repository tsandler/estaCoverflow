#include "pcb.h"
#include "metadata_program.h"
#include "sockets.h"


int algoritmoDePeso(int cantEtq, int cantfc, t_size cantInstruc){

	int peso = 5*cantEtq+ 3*cantfc + cantInstruc;
	return peso;
}


t_pcb* armarPCB(char* program){

	t_metadata_program* metadataP=metadata_desde_literal(program);

	int peso =algoritmoDePeso(metadataP->cantidad_de_etiquetas,metadataP->cantidad_de_funciones,metadataP->instrucciones_size);

	t_pcb* unPCB;

	unPCB=malloc(sizeof(int)*9);

	unPCB->peso = peso;





	return unPCB;


}


