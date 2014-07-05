#include "manejoCPU.h"
#include "colas.h"
#include "manejoIO.h"

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
extern t_dictionary * variablesCompartidas;
extern t_dictionary * dispositivosIO;
extern t_dictionary * semaforos;
extern t_config * config;
extern sem_t mutexSemaforos;
extern sem_t mutexMandarColaEXEC;


extern t_dictionary * dispositivosIO;
registroPCB* PCBrecibido;

bool *condicion(registroPCB* pcb) {
	return pcb->pid == PCBrecibido->pid;
};


void manejoCPU(int fd) {

	puts("corriendo manejo de la cpu");
	int tiempo;
	char* dispositivo;
	char* textoAImprimir;
	char* nombreSem;
	int valorMostrar;
	char* variable;
	int valorRecibido;
	int valorSem;
	bool *habilitado;
	int quantum = config_get_int_value(config, "QUANTUM");
	int retardo = config_get_int_value(config, "RETARDO");
	t_length* tam;
	tam = sizeof(registroPCB);
	PCBrecibido = malloc(sizeof(registroPCB));
	registroPCB* PCBPOP = malloc(sizeof(registroPCB));
	registroPCB* unPCB = malloc(sizeof(registroPCB));

	t_log * logs = log_create("log_PCP_En_CPU", "manejoCPU.c", 0,
			LOG_LEVEL_TRACE);

	unPCB = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //para mandar a exec
	log_info(logs, "Se saco de la cola Ready el proceso %i",unPCB->pid);

	enviarDatos(fd, tam, quantum, logs);
	enviarDatos(fd, tam, retardo, logs);

	sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
	ponerCola(unPCB, EXEC, &mutexEXEC, &hayAlgoEnExec);
	log_info(logs, "Se Coloco en la cola EXEC el PCB %i",unPCB->pid);
	enviarDatos(fd, tam, unPCB, logs);
	sem_post(&mutexMandarColaEXEC);

	while (1) {

		recibirMenu(fd, tam, logs);

		switch (tam->menu) {

		case OBTENER_VALOR_COMPARTIDA: // char*
			recibirDato(fd, tam->length, variable, logs);
			sem_wait(&mutexVarCompartidas);
			int valor = dictionary_get(variablesCompartidas, variable);
			sem_post(&mutexVarCompartidas);
			enviarDatos(fd, tam, valor, logs);

			break;

		case ASIGNAR_VALOR_COMPARTIDA: //char*
			recibirDato(fd, tam->length, valorRecibido, logs);
			recibirDatos(fd, tam, variable, logs);

			sem_wait(&mutexVarCompartidas);
			dictionary_remove(variablesCompartidas, variable);
			log_info(logs,"Se saco la variable compartida %s",variable);
			dictionary_put(variablesCompartidas, variable, valorRecibido);
			log_info(logs,"Se coloco la variable compartida %s y su valor es %i",variable,valorRecibido);
			sem_post(&mutexVarCompartidas);
			break;

		case IMPRIMIR: //int
			recibirDato(fd, tam->length, valorMostrar, logs);

			//creo que esta funcion es asi, no estoy seguro... "GONZA ESTUBO AQUI"
			recibirDatos(fd,tam,valorMostrar,logs);
			enviarDatos(fd,tam,valorMostrar,logs);
			log_info(logs,"el valor a imprimir es %i",valorMostrar);
			// necesito el nombre de la variable TOBI TOBI TOBI ALSDJFLASJFLKDASJFLADSKJ


			break;

		case IMPRIMIR_TEXTO: //char*
			recibirDato(fd, tam->length, textoAImprimir, logs);
			recibirDatos(fd, tam, PCBrecibido, logs);
			enviarDatos(PCBrecibido->fd, tam, textoAImprimir, logs);
			log_info(logs,"el texto a imprimir es %s",textoAImprimir);
			break;

		case ENTRADA_SALIDA: //char* (dispositivo), int (tiempo)

			recibirDato(fd, tam->length, tiempo, logs);
			recibirDatos(fd, tam, dispositivo, logs);
			recibirDatos(fd, tam, PCBrecibido, logs);
			sem_wait(&mutexEXEC);
			EXEC = list_filter(EXEC, *condicion); //saco de la cola exec
			log_info(logs,"Se saco de la cola EXEC el proceso %i",PCBrecibido->pid);
			sem_post(&mutexEXEC);
			t_io*io = dictionary_get(dispositivosIO, dispositivo);
			PCBrecibido->retrasoIO = tiempo;
			ponerCola(PCBrecibido, io->cola, &io->mutex, &io->hayAlgo); //lo mando al io
			log_info(logs,"Se coloco en la Cola de IO el proceso %i",PCBrecibido->pid);
			PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //mando PCB nuevo nuevo.
			sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
			ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
			log_info(logs,"Se coloco en la Cola Ready el proceso %i",PCBPOP->pid );
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutexMandarColaEXEC);

			break;

		case WAIT: //char*

			recibirDato(fd, tam->length, nombreSem, logs);

			//ESTO DEBE SER ATOMICO
			log_info(logs, "El semaforo se llama %s",nombreSem);
			sem_wait(&mutexSemaforos);
			valorSem = dictionary_get(semaforos, nombreSem);
			if (valorSem <= 0) {
				habilitado = false;
				log_info(logs,"y su valor es %i",valorSem);
				enviarDatos(fd, tam, habilitado, logs);
			} else {
				valorSem = valorSem - 1;
				habilitado = true;
				enviarDatos(fd, tam, habilitado, logs);
				log_info(logs,"y su valor es %i",valorSem);
				dictionary_remove(semaforos, nombreSem);
				dictionary_put(semaforos, nombreSem, valorSem);
			}
			sem_post(&mutexSemaforos);

			break;
		case SIGNAL: //char*
			recibirDato(fd, tam->length, nombreSem, logs);
			log_info(logs, "El semaforo es %s",nombreSem);
			sem_wait(&mutexSemaforos);
			valorSem = dictionary_get(semaforos, nombreSem);
			valorSem = valorSem + 1;
			log_info(logs,"y su valor es %i",valorSem);
			dictionary_remove(semaforos, nombreSem);
			dictionary_put(semaforos, nombreSem, valorSem);
			sem_post(&mutexSemaforos);

			break;
		case CONCLUYO_UN_QUANTUM:

			recibirDato(fd, tam->length, PCBrecibido, logs);
			log_info(logs,"Se saca de EXEC el proceso %i",PCBrecibido->pid);
			//saco de exec el PCBrecibido

			sem_wait(&mutexEXEC);
			EXEC = list_filter(EXEC, *condicion); //saco de la cola exec
			log_info(logs,"se Saco de la cola EXEC el procecso %i",PCBrecibido->pid);
			sem_post(&mutexEXEC);

			ponerCola(PCBrecibido, READY, &mutexREADY, &hayAlgoEnReady); //lo pongo en ready
			log_info(logs,"Se coloco en la cola Ready el proceso %i",PCBrecibido->pid);
			PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //saco de ready
			log_info(logs,"Se saco de la cola Ready el proceso %i",PCBrecibido->pid);
			sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
			ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
			log_info(logs,"Se coloco en la cola EXEC el proceso %i",PCBrecibido->pid);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutexMandarColaEXEC);
			break;

		case FINALIZAR:
			recibirDato(fd, tam->length, PCBrecibido, logs);
			sem_wait(&mutexEXEC);
			EXEC = list_filter(EXEC, *condicion); //saco de la cola exec
			log_info(logs,"Se saco de la cola EXEC el proceso %i",PCBrecibido->pid);
			sem_post(&mutexEXEC);

			ponerCola(PCBrecibido, EXIT, &mutexEXIT, &hayAlgoEnExit); //la pongo en exit
			log_info(logs,"Se agrego a la cola EXIT el proceso %i",PCBrecibido->pid);
			enviarMenu(PCBrecibido->fd, tam, logs); //aviso al programa q finalizo.
			log_info(logs,"El programa ah finalizado");

			PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //mando de nuevo.
			sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
			ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
			log_info(logs,"Se coloco en la Cola EXEC el proceso %i",PCBPOP->pid);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutexMandarColaEXEC);
			break;

		default:
			log_error(logs,"Se interrumpio el proceso con el PID: %d debido a que la CPU esta caida",unPCB->pid); //antes habias puesto unPCB->fd
																									 //creo que te confundistes sino lo cambiamos
			sem_wait(&mutexEXEC);
			EXEC = list_filter(EXEC, *condicion); //saco de la cola exec
			log_info(logs,"Se saco de la cola Exec el proceso %d",unPCB->pid);
			sem_post(&mutexEXEC);

			//lo mando a exit de una.

			ponerCola(unPCB,EXIT,&mutexEXIT,&hayAlgoEnExit);
			log_info(logs,"Se mando a la cola EXIT el proceso %i",unPCB->pid);
			break;
		}
	}

}

