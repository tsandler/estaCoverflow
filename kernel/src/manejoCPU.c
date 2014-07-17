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
extern t_config * config;
extern sem_t mutexSemaforos;
extern sem_t mutexMandarColaEXEC;
extern int socket_UMV;

extern t_dictionary * dispositivosIO;
registroPCB* PCBrecibido;

bool condicion(registroPCB* pcb) {
	return pcb->pid == PCBrecibido->pid;
}
;

void manejoCPU(int fd) {
	t_log * logs = log_create("log_PCP_En_CPU", "manejoCPU.c", 1,
			LOG_LEVEL_TRACE);
	log_info(logs, "Corriendo manejo del CPU");
	int tiempo;
	char* dispositivo;
	char* textoAImprimir;
	char* nombreSem;
	char* sem;
	int valorMostrar;
	char* variable;
	int valorRecibido;
	bool bloqueado;
	t_semaforos* tSem;
	int quantum = config_get_int_value(config, "QUANTUM");
	int retardo = config_get_int_value(config, "RETARDO");
	t_length* tam = malloc(sizeof(registroPCB));

	PCBrecibido = malloc(sizeof(registroPCB));
	registroPCB* PCBPOP = malloc(sizeof(registroPCB));
	registroPCB* unPCB = malloc(sizeof(registroPCB));
	registroPCB* ULTIMOPCB = malloc(sizeof(registroPCB));
	tam->length = sizeof(int);
	int stack = config_get_int_value(config, "TAMANIO_STACK");
	enviarDatos(fd, tam, &quantum, logs);
	enviarDatos(fd, tam, &stack, logs);
	enviarDatos(fd, tam, &retardo, logs);
	unPCB = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //para mandar a exec
	ULTIMOPCB = unPCB;
	log_info(logs, "Se saco de la cola Ready el proceso %i", unPCB->pid);

	sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
	ponerCola(unPCB, EXEC, &mutexEXEC, &hayAlgoEnExec);
	log_info(logs, "Se Coloco en la cola EXEC el PCB %i", unPCB->pid);
	tam->length = sizeof(registroPCB);
	enviarDatos(fd, tam, unPCB, logs);
	sem_post(&mutexMandarColaEXEC);

	while (1) {

		recibirMenu(fd, tam, logs);

		switch (tam->menu) {
		char* var;
		int fdRecibido;
	case OBTENER_VALOR_COMPARTIDA: // char*
		recibirDato(fd, tam->length, (void*)&variable, logs);
		sem_wait(&mutexVarCompartidas);
		var = string_from_format("%s", &variable);
		int* valor = dictionary_get(variablesCompartidas, var);
		sem_post(&mutexVarCompartidas);
		tam->length = sizeof(int);
		enviarDatos(fd, tam, valor, logs);
		break;

	case ASIGNAR_VALOR_COMPARTIDA: //char*
		recibirDato(fd, tam->length, (void*) &valorRecibido, logs);
		recibirDatos(fd, tam, (void*) &variable, logs);
		sem_wait(&mutexVarCompartidas);
		var = string_from_format("%s", &variable);
		dictionary_remove(variablesCompartidas, var);
		log_info(logs, "Se saco la variable compartida %s", variable);
		dictionary_put(variablesCompartidas, var, &valorRecibido);
		log_info(logs, "Se coloco la variable compartida %s y su valor es %i",
				variable, valorRecibido);
		sem_post(&mutexVarCompartidas);
		break;

	case IMPRIMIR:
		recibirDato(fd, tam->length, (void*) &valorMostrar, logs);
		recibirDatos(fd, tam, (void*) &fdRecibido, logs);
		tam->length = sizeof(int);
		enviarDatos(fdRecibido, tam, &valorMostrar, logs);
		log_info(logs, "El valor a imprimir es %i", valorMostrar);

		break;

	case IMPRIMIR_TEXTO:
		recibirDato(fd, tam->length, (void*) &textoAImprimir, logs);
		recibirDatos(fd, tam, (void*) &fdRecibido, logs);
		char* algto = string_from_format("%s", &textoAImprimir);
		tam->length = strlen(algto) + 1;
		enviarDatos(fdRecibido, tam, algto, logs);
		log_info(logs, "el texto a imprimir es %s", algto);
		break;

	case ENTRADA_SALIDA:
		recibirDato(fd, tam->length, (void*) &tiempo, logs);
		recibirDatos(fd, tam, (void*) &dispositivo, logs);
		recibirDatos(fd, tam, (void*) PCBrecibido, logs);
		sem_wait(&mutexEXEC);
		//EXEC = list_filter(EXEC, (void*)condicion); //saco de la cola exec
		log_info(logs, "Se saco de la cola EXEC el proceso %i",
				PCBrecibido->pid);
		sem_post(&mutexEXEC);
		t_io*io = malloc(sizeof(t_io));
		io = dictionary_get(dispositivosIO, dispositivo);
		PCBrecibido->retrasoIO = tiempo;
		ponerCola(PCBrecibido, io->cola, &io->mutex, &io->hayAlgo); //lo mando al io
		log_info(logs, "Se coloco en la Cola de IO el proceso %i",
				PCBrecibido->pid);
		PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //mando PCB nuevo nuevo.
		sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
		ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
		tam->length = sizeof(registroPCB);
		log_info(logs, "Se coloco en la Cola Ready el proceso %i", PCBPOP->pid);
		enviarDatos(fd, tam, PCBPOP, logs);
		sem_post(&mutexMandarColaEXEC);

		break;

	case WAIT:

		recibirDato(fd, tam->length, (void*) &nombreSem, logs);
		sem = string_from_format("%s", &nombreSem);
		//ESTO DEBE SER ATOMICO
		log_info(logs, "El semaforo se llama %s", sem);
		sem_wait(&mutexSemaforos);
		tSem = dictionary_get(semaforos, sem);
		if (tSem->valor <= 0) {
			tSem->valor = tSem->valor - 1;
			bloqueado = true;
			log_info(logs, "y su valor es %i", tSem->valor);
			tam->length = sizeof(bool);
			enviarDatos(fd, tam, &bloqueado, logs);
			recibirDatos(fd, tam, (void*) PCBrecibido, logs);
			ponerCola(PCBrecibido, tSem->cola, &tSem->mutex, &tSem->hayAlgo);

			PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //mando de nuevo.
			ULTIMOPCB = PCBPOP;
			sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
			ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
			tam->length = sizeof(registroPCB);
			log_info(logs, "Se coloco en la Cola EXEC el proceso %i",
					PCBPOP->pid);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutexMandarColaEXEC);

		} else {
			tSem->valor = tSem->valor - 1;
			bloqueado = false;
			enviarDatos(fd, tam, &bloqueado, logs);
			log_info(logs, "y su valor es %i", tSem->valor);
			dictionary_remove(semaforos, nombreSem);
			dictionary_put(semaforos, nombreSem, tSem);
		}

		sem_post(&mutexSemaforos);

		break;
	case SIGNAL:
		recibirDato(fd, tam->length, (void*) &nombreSem, logs);
		sem = string_from_format("%s", &nombreSem);

		log_info(logs, "El semaforo es %s", sem);
		sem_wait(&mutexSemaforos);
		tSem = dictionary_get(semaforos, sem);
		tSem->valor = tSem->valor + 1;
		if (tSem->valor > 0) {
			if (queue_size(tSem->cola) != 0) {
				PCBPOP = sacarCola(tSem->cola, &tSem->mutex, &tSem->hayAlgo);
				ULTIMOPCB = PCBPOP;
				ponerCola(PCBPOP, READY, &mutexREADY, &hayAlgoEnReady);
			}

		}
		log_info(logs, "y su valor es %i", tSem->valor);
		dictionary_remove(semaforos, nombreSem);
		dictionary_put(semaforos, nombreSem, tSem);
		sem_post(&mutexSemaforos);

		break;
	case CONCLUYO_UN_QUANTUM:

		recibirDato(fd, tam->length, (void*) PCBrecibido, logs);
		log_info(logs, "Se saca de EXEC el proceso %i", PCBrecibido->pid);
		//saco de exec el PCBrecibido

		sem_wait(&mutexEXEC);
		//EXEC = list_filter(EXEC, (void*)condicion); //saco de la cola exec
		log_info(logs, "se Saco de la cola EXEC el procecso %i",
				PCBrecibido->pid);
		sem_post(&mutexEXEC);

		ponerCola(PCBrecibido, READY, &mutexREADY, &hayAlgoEnReady); //lo pongo en ready
		log_info(logs, "Se coloco en la cola Ready el proceso %i",
				PCBrecibido->pid);
		PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //saco de ready
		log_info(logs, "Se saco de la cola Ready el proceso %i",
				PCBrecibido->pid);
		sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
		ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
		log_info(logs, "Se coloco en la cola EXEC el proceso %i",
				PCBrecibido->pid);
		tam->length = sizeof(registroPCB);
		enviarDatos(fd, tam, PCBPOP, logs);
		sem_post(&mutexMandarColaEXEC);
		break;

	case FINALIZAR:
		recibirDato(fd, tam->length, (void*) PCBrecibido, logs);
		sem_wait(&mutexEXEC);
		//EXEC = list_filter(EXEC, (void*)condicion); //saco de la cola exec
		log_info(logs, "Se saco de la cola EXEC el proceso %i",
				PCBrecibido->pid);
		sem_post(&mutexEXEC);

		ponerCola(PCBrecibido, EXIT, &mutexEXIT, &hayAlgoEnExit); //la pongo en exit
		sem_post(&gradoProg);
		eliminarSegmentoUMV(socket_UMV, logs, PCBrecibido);

		log_info(logs, "Se agrego a la cola EXIT el proceso %i",
				PCBrecibido->pid);
		tam->length = sizeof(int);
		enviarMenu(PCBrecibido->fd, tam, logs); //aviso al programa q finalizo.
		log_info(logs, "El programa ha finalizado");

		PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //mando de nuevo.
		ULTIMOPCB = PCBPOP;
		sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
		ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
		tam->length = sizeof(registroPCB);
		log_info(logs, "Se coloco en la Cola EXEC el proceso %i", PCBPOP->pid);
		enviarDatos(fd, tam, PCBPOP, logs);
		sem_post(&mutexMandarColaEXEC);
		break;

	default:
		log_error(logs,
				"Se interrumpio el proceso con el PID: %d debido a que la CPU esta caida",
				unPCB->pid);

		ponerCola(ULTIMOPCB, READY, &mutexREADY, &hayAlgoEnReady);
		log_error(logs, "Se cierra la CPU.");
		pthread_exit(1);

		break;
		}
	}

}

