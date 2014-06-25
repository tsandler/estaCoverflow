#include <commons/collections/queue.h>
#include <semaphore.h>
#include <libs/manejoDelPCB.h>
#include <stdio.h>
#include <unistd.h>



typedef struct t_io{
	char* nombre;
	sem_t hayAlgo;
	sem_t mutex;
	t_queue* cola;
	int retardo;
}t_io;

void manejoIO(t_io* io);

