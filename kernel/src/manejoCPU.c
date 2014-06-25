#include "manejoCPU.h"
#include "colas.h"

extern t_queue* READY;
extern t_queue* EXIT;
extern t_queue* EXEC;
extern sem_t mutexREADY;
extern sem_t mutexEXEC;
extern sem_t mutexEXIT;

extern sem_t hayAlgoEnReady;
extern sem_t hayAlgoEnExec;




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
	enviarDatos(fd, tam, unPCB, logs);
	sem_post(&mutex);

	while (1) {
		registroPCB* PCBrecibido=malloc(sizeof(registroPCB));
		registroPCB* PCBPOP=malloc(sizeof(registroPCB));
		recibirMenu(fd, tam, logs);
		//como recibo, debo sacarlo del exec, NO SE COMO USAR ESTA FC.
		//list_remove_by_condition(EXEC, bool(*condition)(void*));


		switch (tam->menu) {

		/*case OBTENER_VALOR_COMPARTIDA: // char*
			//proceso el pcb y lo pongo en cola de ready de nuevo
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			//saco otro pcb para enviarlo y ejecutarlo
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);

			break;
		case ASIGNAR_VALOR_COMPARTIDA: //char*
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case IMPRIMIR: //int
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case IMPRIMIR_TEXTO: //char*
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case ENTRADA_SALIDA: //char* (dispositivo), int (tiempo)

			// deberiamos filtrar de alguna manera a q io va. arreglar con cpu.


			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case WAIT: //char*
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case SIGNAL: //char*
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;*/
		case CONCLUYO_UN_QUANTUM:
			recibirDato(fd, tam->length, (void*)PCBrecibido, logs);
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case FINALIZAR:
			queue_push(EXIT, PCBrecibido);
			break;
		default:
			printf("ERROR en recv desde la CPU");
			break;
		}
	}

}

