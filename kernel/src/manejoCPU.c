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
}

void manejoCPU(int fd) {

	puts("corriendo manejode la cpu");
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

	enviarDatos(fd, tam, quantum, logs);
	enviarDatos(fd, tam, retardo, logs);

	sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
	ponerCola(unPCB, EXEC, &mutexEXEC, &hayAlgoEnExec);
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
			dictionary_put(variablesCompartidas, variable, valorRecibido);
			sem_post(&mutexVarCompartidas);
			break;

		case IMPRIMIR: //int
			recibirDato(fd, tam->length, valorMostrar, logs);
			// necesito el nombre de la variable TOBI TOBI TOBI ALSDJFLASJFLKDASJFLADSKJ
			//	VER VER VER VER VER VER V E RV ROFL LMAO LOL
			//AGUANTE PIKACHU

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
			ponerCola(PCBrecibido, io->cola, &io->mutex, &io->hayAlgo); //lo mando al io

			PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //mando PCB nuevo nuevo.
			sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
			ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutexMandarColaEXEC);

			break;

		case WAIT: //char*

			recibirDato(fd, tam->length, nombreSem, logs);

			//ESTO DEBE SER ATOMICO

			sem_wait(&mutexSemaforos);
			valorSem = dictionary_get(semaforos, nombreSem);
			if (valorSem <= 0) {
				habilitado = false;
				enviarDatos(fd, tam, habilitado, logs);
			} else {
				valorSem = valorSem - 1;
				habilitado = true;
				enviarDatos(fd, tam, habilitado, logs);
				dictionary_remove(semaforos, nombreSem);
				dictionary_put(semaforos, nombreSem, valorSem);
			}
			sem_post(&mutexSemaforos);

			break;
		case SIGNAL: //char*
			recibirDato(fd, tam->length, nombreSem, logs);
			sem_wait(&mutexSemaforos);
			valorSem = dictionary_get(semaforos, nombreSem);
			valorSem = valorSem + 1;
			dictionary_remove(semaforos, nombreSem);
			dictionary_put(semaforos, nombreSem, valorSem);
			sem_post(&mutexSemaforos);

			break;
		case CONCLUYO_UN_QUANTUM:

			recibirDato(fd, tam->length, PCBrecibido, logs);
			//saco de exec el PCBrecibido

			sem_wait(&mutexEXEC);
			EXEC = list_filter(EXEC, *condicion); //saco de la cola exec
			sem_post(&mutexEXEC);

			ponerCola(PCBrecibido, READY, &mutexREADY, &hayAlgoEnReady); //lo pongo en ready
			PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //saco de ready

			sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
			ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutexMandarColaEXEC);
			break;

		case FINALIZAR:
			recibirDato(fd, tam->length, PCBrecibido, logs);
			sem_wait(&mutexEXEC);
			EXEC = list_filter(EXEC, *condicion); //saco de la cola exec
			sem_post(&mutexEXEC);

			ponerCola(PCBrecibido, EXIT, &mutexEXIT, &hayAlgoEnExit); //la pongo en exit

			enviarMenu(PCBrecibido->fd, tam, logs); //aviso al programa q finalizo.

			PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //mando de nuevo.
			sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
			ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutexMandarColaEXEC);
			break;

		default:
			printf(
					"Se cayo esta CPU, se interrumpio el proceso con el PID: %d \n",
					unPCB->fd);
			sem_wait(&mutexEXEC);
			EXEC = list_filter(EXEC, *condicion); //saco de la cola exec
			sem_post(&mutexEXEC);

			//lo mando a exit de una.

			ponerCola(unPCB,EXIT,&mutexEXIT,&hayAlgoEnExit);

			break;
		}
	}

}

