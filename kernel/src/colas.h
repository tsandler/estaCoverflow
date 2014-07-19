
#include <semaphore.h>
#include <commons/collections/queue.h>
#include <libs/manejoDelPCB.h>

void* sacarCola(t_queue* cola, sem_t *mutex, sem_t* hayAlgo);
void ponerCola(registroPCB *unPCB, t_queue* cola, sem_t *mutex, sem_t* hayAlgo);
void muestraNombres(t_queue *cola);
