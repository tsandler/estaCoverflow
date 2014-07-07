#include "manejoIO.h"
#include "colas.h"
extern t_queue* READY;
extern t_log *logs;


void manejoIO(t_io* io){

	log_info(logs,"%s corriendo dispositivo",io->nombre);
while(1){

	registroPCB* unPCB=malloc(sizeof(registroPCB));//Cambio esto... para buscar el tamaño
	unPCB=sacarCola(io->cola,&io->mutex,&io->hayAlgo);
	log_info(logs,"Termino de entrada y salida el programa: %i",unPCB->pid);

	sleep(unPCB->retrasoIO*io->retardo/1000);

	ponerCola(unPCB, READY,&io->mutex, &io->hayAlgo);
	log_info(logs, "Se coloco en la cola ready el programa: %i",unPCB->pid);




}

}
