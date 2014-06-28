#include "colas.h"

void* sacarCola(t_queue* cola, sem_t *mutex, sem_t *hayAlgo) {
	sem_wait(hayAlgo);
	sem_wait(mutex);
	registroPCB* unPCB = queue_pop(cola);
	printf("peso a procesar %d", unPCB->peso);
	sem_post(mutex);

	return unPCB;
}
void ponerCola(registroPCB *unPCB, t_queue* cola, sem_t *mutex, sem_t *hayAlgo) {
	sem_wait(mutex);
	queue_push(cola, unPCB);
	printf("peso a procesar %d", unPCB->peso);
	sem_post(mutex);
	sem_post(hayAlgo);
}

t_list* list_filter_pid(t_list* self, bool(*condition)(int)){
		t_list* filtered = list_create();

		void _add_if_apply(registroPCB* element) {
			if (condition(element->pid)) {
				list_add(filtered, element->pid);
			}
		}

		list_iterate(self, _add_if_apply);
		return filtered;
	}


