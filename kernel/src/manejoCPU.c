
#include  "manejoCPU.h"

extern t_queue* READY;
extern sem_t mutexREADY;
extern sem_t hayAlgoEnReady;
extern t_queue* EXIT;

registroPCB* sacarCola(t_queue* cola){
	sem_wait(&hayAlgoEnReady);
		sem_wait(&mutexREADY);
		registroPCB* unPCB = queue_pop(cola);
		printf("peso a procesar %d", unPCB->peso);
		sem_post(&mutexREADY);

		return unPCB;
}
void ponerCola(registroPCB *unPCB, t_queue* cola){
	sem_wait(&mutexREADY);
		queue_push(cola,unPCB);
		printf("peso a procesar %d", unPCB->peso);
		sem_post(&mutexREADY);
		sem_post(&hayAlgoEnReady);
}


void manejoCPU(int fd)
{
	t_log * logs = log_create("log_PCP_En_CPU","manejoCPU.c",0,LOG_LEVEL_TRACE);
	registroPCB* unPCB = sacarCola(READY);
	t_length* tam;
	tam->length= sizeof(registroPCB);
	enviarDatos(fd, tam, unPCB, logs);


	while(1){
		registroPCB* PCBrecibido;
		registroPCB* PCBPOP;
		recibirDatos(fd, tam, PCBrecibido, logs);

		switch(tam->menu){

		case OBTENER_VALOR_COMPARTIDA:
			ponerCola(PCBrecibido,READY);
			PCBPOP=sacarCola(READY);
			enviarDatos(fd, tam, PCBPOP, logs);
			break;
		case ASIGNAR_VALOR_COMPARTIDA:
			ponerCola(PCBrecibido,READY);
			PCBPOP=sacarCola(READY);
			enviarDatos(fd, tam, PCBPOP, logs);
			break;
		case PEDIR_INDICE_ETIQUETAS:
			ponerCola(PCBrecibido,READY);
			PCBPOP=sacarCola(READY);
			enviarDatos(fd, tam, PCBPOP, logs);
			break;
		case IMPRIMIR:
			ponerCola(PCBrecibido,READY);
			PCBPOP=sacarCola(READY);
			enviarDatos(fd, tam, PCBPOP, logs);
			break;
		case IMPRIMIR_TEXTO:
			ponerCola(PCBrecibido,READY);
			PCBPOP=sacarCola(READY);
			enviarDatos(fd, tam, PCBPOP, logs);
			break;
		case ENTRADA_SALIDA:
			ponerCola(PCBrecibido,READY);
			PCBPOP=sacarCola(READY);
			enviarDatos(fd, tam, PCBPOP, logs);
			break;
		case WAIT:
			ponerCola(PCBrecibido,READY);
			PCBPOP=sacarCola(READY);
			enviarDatos(fd, tam, PCBPOP, logs);
			break;
		case SIGNAL:
			ponerCola(PCBrecibido,READY);
			PCBPOP=sacarCola(READY);
			enviarDatos(fd, tam, PCBPOP, logs);
			break;
		case CONCLUYO_UN_QUANTUM:
			ponerCola(PCBrecibido,READY);
			PCBPOP=sacarCola(READY);
			enviarDatos(fd, tam, PCBPOP, logs);
			break;
		case FINALIZAR:
			ponerCola(PCBrecibido,EXIT);
			break;
		case LEER_SEGMENTO:
			ponerCola(PCBrecibido,READY);
			PCBPOP=sacarCola(READY);
			enviarDatos(fd, tam, PCBPOP, logs);
			break;
		case PEDIR_SENTENCIA:
			ponerCola(PCBrecibido,READY);
			PCBPOP=sacarCola(READY);
			enviarDatos(fd, tam, PCBPOP, logs);
			break;
		case RETORNO_DE_STACK:
			ponerCola(PCBrecibido,READY);
			PCBPOP=sacarCola(READY);
			enviarDatos(fd, tam, PCBPOP, logs);
			break;
		default:
			printf("ERROR en recv desde la CPU");
			break;
		}
	}

}

