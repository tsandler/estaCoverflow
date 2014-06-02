#include <libs/sockets.h>
#include <pthread.h>
#include "functions.h"

void*print_message_function(void*ptr);
t_log* logs;
int main(int argc, char** argv){
	t_config* config = config_create(argv[1]);
	int port = config_get_int_value(config,"PUERTO_UMV");
	tamanioUMV = config_get_int_value(config, "tamanioUMV");
	int serverUMV;
	int procesoConectado;
	int procesoIdentificado;
	pthread_t thread1,thread2;
	logs= log_create("log","UMV.c",0,LOG_LEVEL_TRACE);
		if (argc < 2){
			log_error(logs, "No se envio ningun parametro");
			log_destroy(logs);
			return 0;
		}

	serverUMV = crearServidor(port,logs);
	if(serverUMV<0){
		log_error(logs,"no se pudo crear el servidor");
		return 0;
	}

	while(1){
		procesoConectado = aceptarConexion(serverUMV,logs);
		int* pConectado=malloc(sizeof(int));
		*pConectado= procesoConectado;

		if(procesoConectado<0){
			log_error(logs,"no se pudo aceptar la conexion correctamente");
			return 0;
		}
		procesoIdentificado= handshake(procesoConectado);
		if(procesoIdentificado ==0){
			log_error(logs,"no se conecto CPU o KERNEL");
			return 0;
		}
		if(procesoIdentificado ==1){
		    escucharCPU(pConectado);
		} else if(procesoIdentificado ==2){
			escucharKernel(pConectado);
		}
	}
	log_destroy(logs);
	config_destroy(config);
	return 0;
	}

void*print_message_function(void*ptr){
	int procesoConectado;
	procesoConectado = (int) ptr;
	printf("%d\n", procesoConectado);
	return NULL;
}



