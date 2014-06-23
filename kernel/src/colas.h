#include <libs/manejoDelPCB.h>
#include <semaphore.h>
#include <commons/collections/queue.h>

void* sacarCola(t_queue* cola, sem_t mutex, sem_t hayAlgo);
void ponerCola(registroPCB *unPCB, t_queue* cola, sem_t mutex, sem_t hayAlgo);
