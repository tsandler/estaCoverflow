#include  "manejoCPU.h"
#include <libs/colas.h>

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
		recibirDatos(fd, tam, PCBrecibido, logs);
		//como recibo, debo sacarlo del exec, NO SE COMO USAR ESTA FC.
		//list_remove_by_condition(EXEC, bool(*condition)(void*));


		switch (tam->menu) {

		case OBTENER_VALOR_COMPARTIDA:
			//proceso el pcb y lo pongo en cola de ready de nuevo
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			//saco otro pcb para enviarlo y ejecutarlo
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);

			break;
		case ASIGNAR_VALOR_COMPARTIDA:
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case PEDIR_INDICE_ETIQUETAS:
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case IMPRIMIR:
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case IMPRIMIR_TEXTO:
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case ENTRADA_SALIDA:

			// deberiamos filtrar de alguna manera a q io va. arreglar con cpu.


			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case WAIT:
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case SIGNAL:
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case CONCLUYO_UN_QUANTUM:
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
		case LEER_SEGMENTO:
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case PEDIR_SENTENCIA:
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		case RETORNO_DE_STACK:
			ponerCola(PCBrecibido, READY, mutexREADY, hayAlgoEnReady);
			PCBPOP = sacarCola(READY, mutexREADY, hayAlgoEnReady);
			sem_wait(&mutex);	//envio datos y pongo en exec atomicamente
			ponerCola(unPCB,EXEC, mutexEXEC, hayAlgoEnExec);
			enviarDatos(fd, tam, PCBPOP, logs);
			sem_post(&mutex);
			break;
		default:
			printf("ERROR en recv desde la CPU");
			break;
		}
	}

}

