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

extern t_dictionary * dispositivosIO;




void manejoCPU(int fd) {

	puts("corriendo manejode la cpu");
	t_log * logs = log_create("log_PCP_En_CPU", "manejoCPU.c", 0,
			LOG_LEVEL_TRACE);
	registroPCB* unPCB = sacarCola(READY, mutexREADY, hayAlgoEnReady);
	t_length* tam;
	tam->length = sizeof(registroPCB);
	sem_t mutex;
	sem_init(&mutex, 0, 1);

	sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
	ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
	enviarDatos(fd, tam, unPCB, logs); //HAY Q MANDAR EL QUANTUM Y EL RETARDO. UNICA VEZ. ARREGLAR CON TOBI
	sem_post(&mutex);
			registroPCB* PCBrecibido = malloc(sizeof(registroPCB));
			registroPCB* PCBPOP = malloc(sizeof(registroPCB));
			registroPCB* PCBIO = malloc(sizeof(registroPCB));
			int tiempo;
			char* dispositivo;
			char* textoAImprimir;
			int valorMostrar;
	while (1) {


		recibirMenu(fd, tam, logs);
		//como recibo, debo sacarlo del exec, NO SE COMO USAR ESTA FC.
		//list_remove_by_condition(EXEC, bool(*condition)(void*));


		switch (tam->menu) {

		case OBTENER_VALOR_COMPARTIDA: // char*


			break;
		case ASIGNAR_VALOR_COMPARTIDA: //char*

			break;
		case IMPRIMIR: //int
			recibirDato(fd,tam->length, valorMostrar, logs);

			//need PCB
			break;
		case IMPRIMIR_TEXTO: //char*
			recibirDato(fd,tam->length, textoAImprimir, logs);
			//necesito PCB
			//enviarDatos(fd, tam, PCBPOP, logs);

			break;
		case ENTRADA_SALIDA: //char* (dispositivo), int (tiempo)

			recibirDato(fd,tam->length, tiempo, logs);
			recibirDatos(fd, tam, dispositivo, logs);
			recibirDatos(fd, tam, PCBIO, logs);
			t_io*io= dictionary_get(dispositivosIO, dispositivo);
			PCBIO->retrasoIO=tiempo;
			ponerCola(PCBIO,io->cola,io->mutex,io->hayAlgo);
			//y tambien sacar del exec... igual q concluyo quantum.

			break;
		case WAIT: //char*

			break;
		case SIGNAL: //char*

			break;
		case CONCLUYO_UN_QUANTUM:


			recibirDato(fd, tam->length, PCBrecibido, logs);
			// hay q sacarlo del exec.. ver.
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(PCBPOP,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case FINALIZAR:

			break;
		default:
			printf("ERROR en recv desde la CPU");
			break;
		}
	}

}

