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
extern sem_t mutexVarCompartidas;
extern t_dictionary * variablesCompartidas;
extern t_config * config;

extern t_dictionary * dispositivosIO;
registroPCB* PCBrecibido;

bool *condicion(registroPCB* pcb) {
	return pcb->pid == PCBrecibido->pid;
}

void manejoCPU(int fd) {

	puts("corriendo manejode la cpu");
	int tiempo;
	char* dispositivo;
	char* textoAImprimir;
	int valorMostrar;
	char* variable;
	int valorRecibido;
	int quantum=config_get_int_value(config,"QUANTUM");
	int retardo=config_get_int_value(config,"RETARDO");
	t_length* tam;
	tam->length = sizeof(registroPCB);
	PCBrecibido = malloc(sizeof(registroPCB));
	registroPCB* PCBPOP = malloc(sizeof(registroPCB));
	registroPCB* unPCB = malloc(sizeof(registroPCB));

	t_log * logs = log_create("log_PCP_En_CPU", "manejoCPU.c", 0,
			LOG_LEVEL_TRACE);


	unPCB = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //para mandar a exec

	enviarDatos(fd, tam, quantum, logs);
	enviarDatos(fd, tam, retardo, logs);

	sem_t mutex;
	sem_init(&mutex, 0, 1);

	sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
	ponerCola(unPCB, EXEC, &mutexEXEC, &hayAlgoEnExec);
	enviarDatos(fd, tam, unPCB, logs);
	sem_post(&mutex);



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
			dictionary_put(variablesCompartidas, variable, valorRecibido);
			sem_post(&mutexVarCompartidas);
			break;


		case IMPRIMIR: //int
			recibirDato(fd, tam->length, valorMostrar, logs);
			// necesito el nombre de la variable

			break;


		case IMPRIMIR_TEXTO: //char*
			recibirDato(fd, tam->length, textoAImprimir, logs);
			recibirDatos(fd, tam, PCBrecibido, logs);
			enviarDatos(PCBrecibido->fd, tam, textoAImprimir, logs);

			break;


		case ENTRADA_SALIDA: //char* (dispositivo), int (tiempo)

			recibirDato(fd, tam->length, tiempo, logs);
			recibirDatos(fd, tam, dispositivo, logs);
			recibirDatos(fd, tam, PCBrecibido, logs);
			sem_wait(&mutexEXEC);
			EXEC = list_filter(EXEC, *condicion); //saco de la cola exec
			sem_post(&mutexEXEC);
			t_io*io = dictionary_get(dispositivosIO, dispositivo);
			PCBrecibido->retrasoIO = tiempo;
			ponerCola(PCBrecibido, io->cola, &io->mutex, &io->hayAlgo);

			break;


		case WAIT: //char*

			break;
		case SIGNAL: //char*

			break;
		case CONCLUYO_UN_QUANTUM:

			recibirDato(fd, tam->length, PCBrecibido, logs);
			//saco de exec el PCBrecibido

			sem_wait(&mutexEXEC);
			EXEC = list_filter(EXEC, *condicion); //saco de la cola exec
			sem_post(&mutexEXEC);

			ponerCola(PCBrecibido, READY, &mutexREADY, &hayAlgoEnReady); //lo pongo en ready
			PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //saco de ready

			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;


		case FINALIZAR:
			recibirDato(fd, tam->length, PCBrecibido, logs);
			sem_wait(&mutexEXEC);
			EXEC = list_filter(EXEC, *condicion); //saco de la cola exec
			sem_post(&mutexEXEC);

			enviarMenu(PCBrecibido->fd, tam, logs);
			break;


		default:
			printf("ERROR en recv desde la CPU");
			break;
		}
	}

}

