/*
 * kernel.c
 *
 *  Created on: 18/06/2014
 *      Author: utnso
 */
#include "globales.h"
#include "planificador_corto_plazo.h"
#include "planificador_largo_plazo.h"
#include "manejoIO.h"

t_config *config;
t_queue *NEW;
t_queue *READY;
t_queue * EXEC;
t_queue *EXIT;
sem_t mutexNEW;
sem_t mutexREADY;
sem_t mutexREADY;
sem_t mutexEXEC;
sem_t mutexEXIT;
sem_t mutexVarCompartidas;

sem_t gradoProg;
sem_t hayAlgo;
sem_t hayAlgoEnReady;
sem_t hayAlgoEnExec;
sem_t hayAlgoEnExit;

t_log *logs;
t_dictionary * dispositivosIO;
t_dictionary * variablesCompartidas;
void plp(void* ptr);
void pcp(void* ptr);



int main(int argc, char **argv) {



	config = config_create("config.cnf");

	NEW = queue_create(); //COLAS
	READY = queue_create();
	BLOCK=queue_create();
	EXIT=queue_create();
	EXEC=queue_create();

	int gradoMultiprogramacion = config_get_int_value(config,"MULTIPROGRAMACION");  //CONFIG
	char** dispositivos=config_get_array_value(config,"ID_HIO");
	char** dispRetardo=config_get_array_value(config,"HIO");
	char** variablesComp= config_get_array_value(config,"VARIABLES_COMPARTIDAS");
	char** valorComp= config_get_array_value(config,"VALOR_VARIABLES_COMPARTIDAS");

	sem_init(&mutexNEW, 0, 1);
	sem_init(&mutexREADY, 0, 1);
	sem_init(&mutexEXEC, 0, 1);
	sem_init(&mutexEXIT, 0, 1);
	sem_init(&gradoProg, 0, gradoMultiprogramacion-1);
	sem_init(&hayAlgo, 0, 0);
	sem_init(&hayAlgoEnReady,0,0);
	sem_init(&hayAlgoEnExec,0,0);
	sem_init(&hayAlgoEnExit,0,0);
	sem_init(&mutexVarCompartidas,0,1);


	dispositivosIO= dictionary_create(); // creo diccionario (?)
	variablesCompartidas = dictionary_create();

	logs = log_create("log_Principal","kernel.c",1,LOG_LEVEL_TRACE); //LOG

	pthread_t thread1, thread2;

	int iret1, iret2;

	iret1 = pthread_create(&thread1, NULL, plp, NULL );  //HILO PLP

	if (iret1){
		log_error(logs,"Error en la creacion del hilo PLP");
		log_destroy(logs);

		exit(EXIT_FAILURE);

	}

	iret2 = pthread_create(&thread2, NULL, pcp, NULL ); //HILO PCP

	if (iret2){
		log_error(logs, "Error en la creacion del hilo PCP");
		log_destroy(logs);

		exit(EXIT_FAILURE);

	}

	int i=0;  // HILOS IO

	while(dispositivos[i] != NULL){
		t_io* io;
		io=malloc(sizeof(t_io));

		io->nombre=dispositivos[i];
		int a = atoi(dispRetardo[i]);
		io->retardo=a;
		io->cola=queue_create();
		sem_init(&io->hayAlgo,0,0);
		sem_init(&io->mutex,0,1);
		dictionary_put(dispositivosIO, io->nombre, io);


		pthread_t threadIO;
		int hiloIO =pthread_create(&threadIO, NULL, manejoIO, (void*)io );  //HILO PLP
i++;
		if (hiloIO){
			log_error(logs,"Error en la creacion del hilo PLP");
			log_destroy(logs);

			exit(EXIT_FAILURE);
		}


	}

	// diccionario de variables compartidas
	int j= 0;

	while(variablesComp[j] != NULL){

		int valor = atoi(valorComp[j]);
		dictionary_put(variablesCompartidas, variablesComp[j], valor);

		j++;
	}

	pthread_join(thread1, NULL ); //esperar a q termine el otro

	pthread_join(thread2, NULL );



	sem_destroy(&mutexNEW);
	sem_destroy(&mutexREADY);
	sem_destroy(&mutexEXEC);
	sem_destroy(&mutexEXIT);
	sem_destroy(&gradoProg);
	sem_destroy(&hayAlgo);
	sem_destroy(&hayAlgoEnReady);
	sem_destroy(&hayAlgoEnExec);
	sem_destroy(&hayAlgoEnExit);
	sem_destroy(&mutexVarCompartidas);

	dictionary_destroy(variablesCompartidas);
	dictionary_destroy(dispositivosIO);
	exit(EXIT_SUCCESS);

	log_destroy(logs);
	config_destroy(config);

	return EXIT_SUCCESS;

}

