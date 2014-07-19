#include "colas.h"

t_log *logs;

void* sacarCola(t_queue* cola, sem_t *mutex, sem_t *hayAlgo) {
	sem_wait(hayAlgo);
	sem_wait(mutex);
	registroPCB* unPCB = queue_pop(cola);
	printf("Peso a procesar %d",unPCB->peso);
	muestraNombres(cola);
	//log_info(logs,"peso a procesar %d",unPCB->peso);
	sem_post(mutex);

	return unPCB;
}
void ponerCola(registroPCB *unPCB, t_queue* cola, sem_t *mutex, sem_t *hayAlgo) {
	sem_wait(mutex);
	queue_push(cola, unPCB);
	printf("Peso a procesar %d",unPCB->peso);
	muestraNombres(cola);
	//log_info(logs,"Peso a procesar %d",unPCB->peso);
	sem_post(mutex);
	sem_post(hayAlgo);
}


void muestraNombres(t_queue *cola) {
	t_queue *colaAux;
	colaAux = queue_create();
	registroPCB* pcb;
	list_add_all(colaAux->elements,cola->elements);
	int tamanio =queue_size(cola);
	int i;
	printf("Total de procesos: %d \n",queue_size(colaAux));
	for(i=0;i<tamanio;i++){
		pcb = queue_pop(colaAux);
		printf ("En la cola esta el PID %d \n",pcb->pid);
	}
	printf ("La cola quedo vacia");
	queue_destroy(colaAux);

}





