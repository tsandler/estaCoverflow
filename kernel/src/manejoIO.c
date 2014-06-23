#include "manejoIO.h"
#include "colas.h"
extern t_queue* READY;



void manejoIO(t_io* io){


	printf("%s corriendo  dispositivo \n", io->nombre);
while(1){

	registroPCB* unPCB=malloc(sizeof(registroPCB));
	unPCB=sacarCola(io->cola,io->mutex,io->hayAlgo);

	sleep(unPCB->retrasoIO);

	ponerCola(unPCB, READY,io->mutex, io->hayAlgo);




}

}
