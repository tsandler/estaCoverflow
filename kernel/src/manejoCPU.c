
#include  "manejoCPU.h"

extern t_queue* READY;
extern sem_t mutexREADY;
extern sem_t hayAlgoEnReady;

void manejoCPU(int fd)
{
	sem_wait(&hayAlgoEnReady);
	sem_wait(&mutexREADY);
	registroPCB* unPCB = queue_pop(READY);
	printf("peso a procesar %d", unPCB->peso);
	sem_post(&mutexREADY);


	// WHILE (1)  recv, con lo q recibe se hace un switch.


}


