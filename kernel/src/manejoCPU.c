#include "manejoCPU.h"
#include "colas.h"
#include "manejoIO.h"
#include "funcionesPCB.h"
#include "pthread.h"
#include "globales.h"

extern t_queue* READY;
extern t_queue* EXIT;
extern t_queue* EXEC;
extern sem_t mutexREADY;
extern sem_t mutexEXEC;
extern sem_t mutexEXIT;
extern sem_t hayAlgoEnReady;
extern sem_t hayAlgoEnExec;
extern sem_t hayAlgoEnExit;
extern sem_t mutexVarCompartidas;
extern sem_t gradoProg;
extern t_dictionary * variablesCompartidas;
extern t_dictionary * dispositivosIO;
extern t_dictionary * semaforos;
extern t_dictionary * fileDescriptors;
extern t_config * config;
extern sem_t mutexSemaforos;
extern sem_t mutexMandarColaEXEC;
extern int socket_UMV;

extern t_dictionary * dispositivosIO;

extern sem_t mutexFinalizar;

//
//bool condicion(registroPCB* pcb) {
//	return pcb->pid == PCBrecibido->pid;
//}

void manejoCPU(int fd) {
	registroPCB* PCBrecibido;
	t_log * logs = log_create("log_PCP_En_CPU", "manejoCPU.c", 0,
			LOG_LEVEL_TRACE);
	log_info(logs, "Corriendo manejo del CPU");
	int* tiempo;
	char* dispositivo;
	char* textoAImprimir;
	char* nombreSem;
	int* valorMostrar;
	char* variable;
	int bloqueado;
	t_semaforos* tSem;
	int* valorCompartida = malloc(sizeof(int));
	int quantum = config_get_int_value(config, "QUANTUM");
	int retardo = config_get_int_value(config, "RETARDO");
	t_length* tam = malloc(sizeof(registroPCB));

	t_dictionary *pcbCPU;
	pcbCPU = dictionary_create();
	char* fdMal;

	registroPCB* PCBPOP = malloc(sizeof(registroPCB));
	registroPCB* ULTIMOPCB = malloc(sizeof(registroPCB));
	tam->length = sizeof(int);
	int stack = config_get_int_value(config, "TAMANIO_STACK");
	enviarDatos(fd, tam, &quantum, logs);
	enviarDatos(fd, tam, &stack, logs);
	enviarDatos(fd, tam, &retardo, logs);
	/*unPCB = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //para mandar a exec
	muestraNombres(READY, "READY");
	ULTIMOPCB = unPCB;
	log_info(logs, "Se saco de la cola Ready el proceso %i", unPCB->pid);

	sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
	ponerCola(unPCB, EXEC, &mutexEXEC, &hayAlgoEnExec);
	muestraNombres(EXEC, "EXEC");
	log_info(logs, "Se Coloco en la cola EXEC el PCB %i", unPCB->pid);
	tam->length = sizeof(registroPCB);
	if (!enviarDatos(fd, tam, unPCB, logs)) {
		ponerCola(unPCB, READY, &mutexREADY, &hayAlgoEnReady);
		sem_post(&mutexMandarColaEXEC);
		int r = 1;
		pthread_exit(&r);
	}

	sem_post(&mutexMandarColaEXEC);*/
	t_log* logss = log_create("logggg", "Cpu", 1, LOG_LEVEL_TRACE);
	while (1) {
		tam->menu = OK;
		recibirMenu(fd, tam, logs);
		registroPCB* pcb = malloc(sizeof(registroPCB));
		switch (tam->menu) {
		int* pidRecibido;
	case OBTENER_VALOR_COMPARTIDA: // char*
		recibirDato(fd, tam->length, (void*) &variable, logs);
		sem_wait(&mutexVarCompartidas);
		int* valor = dictionary_get(variablesCompartidas, variable);
		sem_post(&mutexVarCompartidas);
		tam->length = sizeof(int);
		enviarDatos(fd, tam, valor, logs);
		break;

	case ASIGNAR_VALOR_COMPARTIDA: //char*
		recibirDato(fd, tam->length, (void*)&valorCompartida, logs);
		recibirDatos(fd, tam, (void*) &variable, logs);
		sem_wait(&mutexVarCompartidas);
		dictionary_remove(variablesCompartidas, variable);
		log_info(logs, "Se saco la variable compartida %s", variable);
		dictionary_put(variablesCompartidas, variable, valorCompartida);
		log_info(logs, "Se coloco la variable compartida %s y su valor es %i",
				variable, *valorCompartida);
		sem_post(&mutexVarCompartidas);
		break;

	case IMPRIMIR:
		recibirDato(fd, tam->length, (void*) &valorMostrar, logs);
		recibirDatos(fd, tam, (void*) &pidRecibido, logs);
		tam->length = sizeof(int);
		char* p = string_from_format("%d", *pidRecibido);
		int* fdTemp = dictionary_get(fileDescriptors, p);
		enviarDatos(*fdTemp, tam, valorMostrar, logs);
		log_info(logs, "El valor a imprimir es %i", *valorMostrar);

		break;

	case IMPRIMIR_TEXTO:
		recibirDato(fd, tam->length, (void*) &textoAImprimir, logs);
		recibirDatos(fd, tam, (void*) &pidRecibido, logs);
		tam->length = strlen(textoAImprimir) + 1;
		char* p1 = string_from_format("%d", *pidRecibido);
		int* fdTemp1 = dictionary_get(fileDescriptors, p1);
		enviarDatos(*fdTemp1, tam, textoAImprimir, logs);
		log_info(logs, "el texto a imprimir es %s", textoAImprimir);
		break;

	case ENTRADA_SALIDA:
		PCBrecibido = malloc(sizeof(registroPCB));
		recibirDato(fd, tam->length, (void*) &tiempo, logs);
		recibirDatos(fd, tam, (void*) &dispositivo, logs);
		recibirDatos(fd, tam, (void*) &PCBrecibido, logs);

		t_io*io;
		io = dictionary_get(dispositivosIO, dispositivo);
		PCBrecibido->retrasoIO = *tiempo;
		ponerCola(PCBrecibido, io->cola, &io->mutex, &io->hayAlgo); //lo mando al io
		fdMal = string_from_format("%d", fd);
		ULTIMOPCB = dictionary_remove(pcbCPU, fdMal);
		log_info(logs, "Se coloco en la Cola de IO el proceso %i",
				PCBrecibido->pid);
		/*if (!finaliza) {
			PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //mando PCB nuevo nuevo.
			sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
			ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
			tam->length = sizeof(registroPCB);
			log_info(logs, "Se coloco en la Cola Ready el proceso %i",
					PCBPOP->pid);
			enviarDatos(fd, tam, PCBPOP, logs);
			fdRecibido = string_from_format("%d", fd);
			dictionary_put(pcbCPU, fdRecibido, PCBPOP);
			sem_post(&mutexMandarColaEXEC);
		}*/

		break;

	case WAIT:

		recibirDato(fd, tam->length, (void*) &nombreSem, logs);
		log_info(logs, "SYSCALL: WAIT");
		//ESTO DEBE SER ATOMICO
		log_info(logs, "El semaforo se llama %s", nombreSem);
		sem_wait(&mutexSemaforos);
		tSem = dictionary_get(semaforos, nombreSem);
		if (tSem->valor <= 0) {
			tSem->valor = tSem->valor - 1;
			bloqueado = 1;
			log_info(logs, "y su valor es %i", tSem->valor);
			tam->length = sizeof(int);
			enviarDatos(fd, tam, &bloqueado, logs);
			recibirDatos(fd, tam, (void*)&pcb, logs);
			ponerCola(pcb, tSem->cola, &tSem->mutex, &tSem->hayAlgo);
			fdMal = string_from_format("%d", fd);
			ULTIMOPCB = dictionary_remove(pcbCPU, fdMal);
			muestraNombres(tSem->cola, "WAIT BLOQUEADOS");
			sem_post(&mutexSemaforos);
			/*if (!finaliza) {
				PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //mando de nuevo.
				muestraNombres(READY, "READY ");
				ULTIMOPCB = PCBPOP;
				sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
				ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
				tam->length = sizeof(registroPCB);
				log_info(logs, "Se coloco en la Cola EXEC el proceso %i",
						PCBPOP->pid);
				enviarDatos(fd, tam, PCBPOP, logs);
				char* fdRecibido = string_from_format("%d", fd);
				dictionary_put(pcbCPU, fdRecibido, PCBPOP);
				sem_post(&mutexMandarColaEXEC);
			}*/

		} else {
			tSem->valor = tSem->valor - 1;
			bloqueado = 0;
			enviarDatos(fd, tam, &bloqueado, logs);
			log_info(logs, "y su valor es %i", tSem->valor);
			sem_post(&mutexSemaforos);
		}

		break;
	case SIGNAL:

		log_info(logs, "SYSCALL: SIGNAL");
		recibirDato(fd, tam->length, (void*) &nombreSem, logs);

		log_info(logs, "El semaforo es %s", nombreSem);
		sem_wait(&mutexSemaforos);
		tSem = dictionary_get(semaforos, nombreSem);
		tSem->valor = tSem->valor + 1;
		if (!queue_is_empty(tSem->cola)) {

			PCBPOP = sacarCola(tSem->cola, &tSem->mutex, &tSem->hayAlgo);
			muestraNombres(tSem->cola, "SIGNAL BLOQUEADOS");
			ULTIMOPCB = PCBPOP;

			printf("voya poner en ready el PID desbloqueado");

			ponerCola(PCBPOP, READY, &mutexREADY, &hayAlgoEnReady);
			muestraNombres(READY, "READY");

		}
		log_info(logs, "y su valor es %i", tSem->valor);
		sem_post(&mutexSemaforos);

		break;
	case CONCLUYO_UN_QUANTUM:

		recibirDato(fd, tam->length, (void*)&pcb, logs);

		ponerCola(pcb, READY, &mutexREADY, &hayAlgoEnReady); //lo pongo en ready
		muestraNombres(READY, "READY");
		fdMal = string_from_format("%d", fd);
		ULTIMOPCB = dictionary_remove(pcbCPU, fdMal);
		log_info(logs, "Se coloco en la cola Ready el proceso %i", pcb->pid);
		log_debug(logss, "El FD del cpu es %d", fd);
		/*if (!finaliza){
		 PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //saco de ready
		 muestraNombres(READY, "QUATUM READY ");
		 ULTIMOPCB = PCBPOP;
		 log_info(logs, "Se saco de la cola Ready el proceso %i",
		 PCBPOP->pid);
		 sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
		 ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
		 log_info(logs, "Se coloco en la cola EXEC el proceso %i",
		 PCBPOP->pid);
		 tam->length = sizeof(registroPCB);
		 char* fdRecibido = string_from_format("%d", fd);
		 if (!enviarDatos(fd, tam, PCBPOP, logs)) {
		 ponerCola(PCBPOP, READY, &mutexREADY, &hayAlgoEnReady);
		 sem_post(&mutexMandarColaEXEC);
		 dictionary_remove(pcbCPU,fdRecibido);
		 int r = 1;
		 pthread_exit(&r);
		 }

		 dictionary_put(pcbCPU, fdRecibido, PCBPOP);

		 sem_post(&mutexMandarColaEXEC);
		 }*/
		break;

	case FINALIZAR:
		sem_wait(&mutexFinalizar);
		PCBrecibido = malloc(sizeof(registroPCB));
		recibirDato(fd, tam->length, (void*)&PCBrecibido, logs);

		fdMal = string_from_format("%d", fd);
		ULTIMOPCB = dictionary_remove(pcbCPU, fdMal);

		eliminarSegmentoUMV(socket_UMV, logs, PCBrecibido);
		ponerCola(PCBrecibido, EXIT, &mutexEXIT, &hayAlgoEnExit);
		sem_post(&gradoProg);

		log_info(logs, "Se agrego a la cola EXIT el proceso %i",
				PCBrecibido->pid);
		tam->length = sizeof(int);
		char* pidR = string_from_format("%d", PCBrecibido->pid);
		int* fdTemporal = dictionary_get(fileDescriptors, pidR);
		enviarMenu(*fdTemporal, tam, logs); //aviso al programa q finalizo.
		dictionary_remove(fileDescriptors, pidR);
		log_info(logs, "El programa ha finalizado");
		sem_post(&mutexFinalizar);
		/*	if (!finaliza) {
		 PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //mando de nuevo.
		 //	ULTIMOPCB = PCBPOP;
		 sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
		 ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
		 tam->length = sizeof(registroPCB);
		 log_info(logs, "Se coloco en la Cola EXEC el proceso %i",
		 PCBPOP->pid);
		 if (!enviarDatos(fd, tam, PCBPOP, logs)) {
		 ponerCola(PCBPOP, READY, &mutexREADY, &hayAlgoEnReady);
		 sem_post(&mutexMandarColaEXEC);
		 dictionary_remove(pcbCPU,fdRecibido);
		 int r = 1;
		 pthread_exit(&r);
		 }
		 fdRecibido = string_from_format("%d", fd);
		 dictionary_put(pcbCPU, fdRecibido, PCBPOP);
		 sem_post(&mutexMandarColaEXEC);
		 }
		 */
		break;
	case PEDIR_PCB:
		PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //mando de nuevo.
		//	ULTIMOPCB = PCBPOP;
		char* fdRecibido = string_from_format("%d", fd);
		dictionary_put(pcbCPU, fdRecibido, PCBPOP);
		sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
		ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
		tam->length = sizeof(registroPCB);
		log_info(logs, "Se coloco en la Cola EXEC el proceso %i", PCBPOP->pid);
		if (!enviarDatos(fd, tam, PCBPOP, logs)) {
			ponerCola(PCBPOP, READY, &mutexREADY, &hayAlgoEnReady);
			sem_post(&mutexMandarColaEXEC);
			dictionary_remove(pcbCPU, fdRecibido);
			int r = 1;
			pthread_exit(&r);
		}
		fdRecibido = string_from_format("%d", fd);
		dictionary_put(pcbCPU, fdRecibido, PCBPOP);
		sem_post(&mutexMandarColaEXEC);
		break;
	case ERROR:
		fdMal = string_from_format("%d", fd);
		ULTIMOPCB = dictionary_remove(pcbCPU, fdMal);
		log_error(logs, "Se cierra la CPU.");
		int ret = 1;
		pthread_exit(&ret);
	default:
		fdMal = string_from_format("%d", fd);
		ULTIMOPCB = dictionary_remove(pcbCPU, fdMal);
		eliminarSegmentoUMV(socket_UMV, logs, ULTIMOPCB);
		sem_post(&gradoProg);

		ponerCola(ULTIMOPCB, EXIT, &mutexEXIT, &hayAlgoEnExit);

		char* p2 = string_from_format("%d", ULTIMOPCB->pid);
		int* fd2 = dictionary_get(fileDescriptors, p2);
		tam->menu = ERROR;
		enviarMenu(*fd2, tam, logs);
		log_error(logs,"Se interrumpio el proceso con el PID: %d debido a que la CPU esta caida",
				ULTIMOPCB->pid);
		log_error(logs, "Se cierra la CPU.");
		int retorno = 1;
		pthread_exit(&retorno);

		break;
		}
	}

}

