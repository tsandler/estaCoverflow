/*
 ============================================================================
 Name        : kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include "planificador_largo_plazo.h"
#include "planificador_corto_plazo.h"
#include <commons/config.h>
#include <commons/string.h>

#include <memory.h>
#include <stdlib.h>
#include <commons/collections/queue.h>
#include <semaphore.h>


t_config *config;
t_queue *NEW;
t_queue *READY;
sem_t mutexNEW;
sem_t mutexREADY;
sem_t gradoProg;
sem_t hayAlgo;

int main(int argc, char *argv[]) {

/////////////////////////////////////////////////////////////////////////
	char* str2;
	str2 = "/home/utnso/git/tp-2014-1c-out-of-range/kernel/";
	char * str3 = (char *) malloc(
			1 + strlen("config.cnf") + strlen(str2));
	strcpy(str3, str2);
	strcat(str3, "config.cnf");

/////////////////////////////////////////////////////////////////////////

	config = config_create(str3);
	free(str3);
	NEW = queue_create();
	READY = queue_create();
	int gradoMultiprogramacion = config_get_int_value(config,"MULTIPROGRAMACION");
	sem_init(&mutexNEW, 0, 1);
	sem_init(&mutexREADY, 0, 1);
	sem_init(&gradoProg, 0, gradoMultiprogramacion-1);
	sem_init(&hayAlgo, 0, 0);

	pthread_t thread1, thread2;

	int iret1, iret2;

	iret1 = pthread_create(&thread1, NULL, plp, NULL );

	if (iret1)

	{

		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret1);

		exit(EXIT_FAILURE);

	}

	iret2 = pthread_create(&thread2, NULL, pcp, NULL );

	if (iret2)

	{

		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret2);

		exit(EXIT_FAILURE);

	}

	pthread_join(thread1, NULL ); //esperar a q termine el otro

	pthread_join(thread2, NULL );

	sem_destroy(&hayAlgo);
	sem_destroy(&mutexNEW);
	sem_destroy(&mutexREADY);
	sem_destroy(&gradoProg);

	exit(EXIT_SUCCESS);

	return EXIT_SUCCESS;

}







/*
#include "globales.h"

//ESTRUCTURAS


void *planificador_largo_plazo(void* auxPLP);
void *planificador_corto_plazo(void* auxPCP);
void crearSemaforos(char** nombre_semaforos, char** valores_semaforos);

int main() {

	PLP auxPLP;
	PCP auxPCP;
	t_log *logs;
	logs = log_create("log_Principal","kernel.c",0,LOG_LEVEL_TRACE);
	auxPLP.logs = logs;
	auxPCP.logs = logs;
	t_config *config = config_create("config.cnf");
	int port = config_get_int_value(config, "PUERTO_PROGRAM");
	int socket_programa = crearServidor(port, auxPLP.logs);//creo el servidor
	char** nombre_semaforos = config_get_array_value(config, "SEMAFOROS");
	char** valores_semaforos = config_get_array_value(config, "VALOR_SEMAFORO");
	int valor_multiprogramacion = config_get_int_value(config,"MULTIPROGRAMACION");
	tam->menu = DEFINIR_VARIABLE;
	tam->length = sizeof(programa);
	pthread_t thr1 , thr2;
	int r1 , r2;

    crearSemaforos(nombre_semaforos,valores_semaforos);

	//Creo el semaforo

	if((sem_multiprogramacion = sem_open("/semaforo_multiprogramacion", O_CREAT ,0644, valor_multiprogramacion)) == (sem_t*)-1)
		{ perror ("No se puede crear el semaforo"); goto terminar_proceso; };

	if(socket_programa < 0 ){
		log_error(logs, "El servidor no se creo correctamente");
	}else{
		colaNew = list_create(); //creo la cola New que voy a usar
		colaReady = list_create(); //creo la cola Ready
		socket_programa = aceptarConexion(socket_programa , logs); //acepto que el puerto programa se conecte
		if (socket_programa < 0){//verifico que la conexion haya sido exitosa
			log_error(logs,"El servidor no acepto la conexion entrante");
			config_destroy(config);
			log_destroy(logs);
		}else{ //recibo los datos que el programa me envia
				socket_programa = recibirDatos(socket_programa, tam, (void*)&programa, logs);
				if(socket_programa == 0){
					log_error(logs,"El servidor no pudo recibir los datos");
				}else{//creo el hilo del PLP y el hilo del PCP
						auxPLP.programa = programa;
						r1 = pthread_create( &thr1, NULL, planificador_largo_plazo, (void*) &auxPLP);
						r2 = pthread_create( &thr2, NULL, planificador_corto_plazo, (void*) &auxPCP);
						if (r1 != 0){
								log_error(auxPLP.logs, "Se produjo un error en el Programador_Largo_Plazo");
									}
						if (r2 != 0){
								log_error(auxPCP.logs, "Se produjo un error en el Programador_Corto_Plazo");
						}
					}
			}
		}


	sem_unlink("/semaforo_multiprogramacion");
	terminar_proceso:
	config_destroy(config);
	log_destroy(logs);
	log_destroy(auxPLP.logs);
	return 0;
};


// creo todos los semaforos enviados por archivo de configuracion
void crearSemaforos(char** nombre_semaforos, char** valores_semaforos){
	int cantidad_nombres_sem;
	int cantidad_valores_sem;
	cantidad_nombres_sem =sizeof(nombre_semaforos);
	cantidad_valores_sem =sizeof(valores_semaforos);
	int i;
	int valor_semaforo;
	sem_t *sem_cont;
	char nombre_semaforo;

	if (cantidad_nombres_sem == cantidad_valores_sem)
		{for(i = 0 ; i<= cantidad_nombres_sem ;i++ ){
			if (nombre_semaforo != '\0'){
				valor_semaforo = *valores_semaforos[i];
				nombre_semaforo = *nombre_semaforos[i];
				char* nombre_sem = string_from_format("\\" , nombre_semaforo);
				if((sem_cont  = sem_open(nombre_sem, O_CREAT ,0644, valor_semaforo)) == (sem_t*)-1)
					{ perror ("No se puede crear el semaforo"); exit(1); };}
			}
	}else{
		perror ("La cantidad de nombres de semaforos es distinta a la cantidad de valores de semaforos" );
		exit(1);

	}
}

*/
