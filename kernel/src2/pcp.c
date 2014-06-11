#include "pcp.h"
#include "sockets.h"
#include <commons/config.h>
#include <commons/collections/queue.h>

extern t_config *config;
extern t_queue *NEW;
extern t_queue *READY;

t_queue *EXEC;
t_queue *BLOCK;
t_queue *EXIT;

void pcp(void* ptr){
	EXEC = queue_create();
	BLOCK = queue_create();
	EXIT = queue_create();

	int port=config_get_int_value(config, "PUERTO_CPU");
		openSocketServerPCP(port);
}
