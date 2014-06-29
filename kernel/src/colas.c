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

t_list* list_filter_pid(t_list* self, bool(*condition)(int)){
	t_list* filtered = list_create();

	void _add_if_apply(pid) {
	registroPCB* element;
		if (pid == (element->pid)) {
			list_add(filtered, element);
		}
	}
	list_iterate(self, _add_if_apply);
	return filtered;
}


