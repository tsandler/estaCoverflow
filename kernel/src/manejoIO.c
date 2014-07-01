#include "manejoIO.h"
#include "colas.h"
extern t_queue* READY;
extern t_log *logs;


void manejoIO(t_io* io){

	log_info(logs,"%s corriendo dispositivo",io->nombre);
while(1){

	registroPCB* unPCB=malloc(sizeof(t_length));//(registroPCB)); Cambio esto... para buscar el tamaño
	unPCB=sacarCola(io->cola,&io->mutex,&io->hayAlgo);

	sleep(unPCB->retrasoIO*io->retardo/1000);

	ponerCola(unPCB, READY,&io->mutex, &io->hayAlgo);




}

}
