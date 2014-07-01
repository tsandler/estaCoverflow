#include "colas.h"

extern t_log *logs;

void* sacarCola(t_queue* cola, sem_t *mutex, sem_t *hayAlgo) {
	sem_wait(hayAlgo);
	sem_wait(mutex);
	registroPCB* unPCB = queue_pop(cola);
	log_info(logs,"peso a procesar %d",unPCB->peso);
	sem_post(mutex);

	return unPCB;
}
void ponerCola(registroPCB *unPCB, t_queue* cola, sem_t *mutex, sem_t *hayAlgo) {
	sem_wait(mutex);
	queue_push(cola, unPCB);
	log_info(logs,"Peso a procesar %d",unPCB->peso);
	sem_post(mutex);
	sem_post(hayAlgo);
}

