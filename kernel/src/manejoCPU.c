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
registroPCB* PCBrecibido;

bool condicion(registroPCB* pcb) {
	return pcb->pid == PCBrecibido->pid;
}
;

void manejoCPU(int fd) {
	t_log * logs = log_create("log_PCP_En_CPU", "manejoCPU.c", 0,
			LOG_LEVEL_TRACE);
	log_info(logs, "Corriendo manejo del CPU");
	int tiempo;
	char* dispositivo;
	char* textoAImprimir;
	char* nombreSem;
	char* sem;
	int valorMostrar;
	char* variable;
	int bloqueado;
	t_semaforos* tSem;
	int* valorCompartida = malloc(sizeof(int));
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
	muestraNombres(READY,"READY");
	ULTIMOPCB = unPCB;
	log_info(logs, "Se saco de la cola Ready el proceso %i", unPCB->pid);

	sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
	ponerCola(unPCB, EXEC, &mutexEXEC, &hayAlgoEnExec);
	muestraNombres(EXEC,"EXEC");
	log_info(logs, "Se Coloco en la cola EXEC el PCB %i", unPCB->pid);
	tam->length = sizeof(registroPCB);
	enviarDatos(fd, tam, unPCB, logs);
	sem_post(&mutexMandarColaEXEC);
	t_log* logss = log_create("logggg", "Cpu", 1, LOG_LEVEL_TRACE);
	while (1) {
		tam->menu = OK;
		recibirMenu(fd, tam, logs);
		registroPCB* pcb = malloc(sizeof(registroPCB));
		switch (tam->menu) {
			char* var;
			int pidRecibido;
			case OBTENER_VALOR_COMPARTIDA: // char*
				recibirDato(fd, tam->length, (void*) &variable, logs);
				sem_wait(&mutexVarCompartidas);
				var = string_from_format("%s", &variable);
				int* valor = dictionary_get(variablesCompartidas, var);
				sem_post(&mutexVarCompartidas);
				tam->length = sizeof(int);
				enviarDatos(fd, tam, valor, logs);
				break;

			case ASIGNAR_VALOR_COMPARTIDA: //char*
				recibirDato(fd, tam->length, (void*) valorCompartida, logs);
				recibirDatos(fd, tam, (void*) &variable, logs);
				sem_wait(&mutexVarCompartidas);
				var = string_from_format("%s", &variable);
				dictionary_remove(variablesCompartidas, var);
				log_info(logs, "Se saco la variable compartida %s", var);
				dictionary_put(variablesCompartidas, var, valorCompartida);
				log_info(logs, "Se coloco la variable compartida %s y su valor es %i",
						var, *valorCompartida);
				sem_post(&mutexVarCompartidas);
				break;

			case IMPRIMIR:
				recibirDato(fd, tam->length, (void*) &valorMostrar, logs);
				recibirDatos(fd, tam, (void*) &pidRecibido, logs);
				tam->length = sizeof(int);
				char* p = string_from_format("%d", pidRecibido);
				int* fdTemp = dictionary_get(fileDescriptors, p);
				enviarDatos(*fdTemp, tam, &valorMostrar, logs);
				log_info(logs, "El valor a imprimir es %i", valorMostrar);

				break;

			case IMPRIMIR_TEXTO:
				recibirDato(fd, tam->length, (void*) &textoAImprimir, logs);
				recibirDatos(fd, tam, (void*) &pidRecibido, logs);
				char* algto = string_from_format("%s", &textoAImprimir);
				tam->length = strlen(algto) + 1;
				char* p1 = string_from_format("%d", pidRecibido);
				int* fdTemp1 = dictionary_get(fileDescriptors, p1);
				enviarDatos(*fdTemp1, tam, algto, logs);
				log_info(logs, "el texto a imprimir es %s", algto);
				break;

			case ENTRADA_SALIDA:
				recibirDato(fd, tam->length, (void*) &tiempo, logs);
				recibirDatos(fd, tam, (void*) &dispositivo, logs);
				recibirDatos(fd, tam, (void*) PCBrecibido, logs);
				/*sem_wait(&mutexEXEC);
				EXEC = list_filter(EXEC, (void*)condicion); //saco de la cola exec
				log_info(logs, "Se saco de la cola EXEC el proceso %i",
						PCBrecibido->pid);
				sem_post(&mutexEXEC);*/
				char* disp = string_from_format("%s", &dispositivo);
				t_io*io;
				io = dictionary_get(dispositivosIO, disp);
				PCBrecibido->retrasoIO = tiempo;
				ponerCola(PCBrecibido, io->cola, &io->mutex, &io->hayAlgo); //lo mando al io
				log_info(logs, "Se coloco en la Cola de IO el proceso %i",PCBrecibido->pid);
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
				log_info(logs, "SYSCALL: WAIT");
				sem = string_from_format("%s", &nombreSem);
				//ESTO DEBE SER ATOMICO
				log_info(logs, "El semaforo se llama %s", sem);
				sem_wait(&mutexSemaforos);
				tSem = dictionary_get(semaforos, sem);
				if (tSem->valor <= 0) {
					tSem->valor = tSem->valor - 1;
					bloqueado = 1;
					log_info(logs, "y su valor es %i", tSem->valor);
					tam->length = sizeof(int);
					enviarDatos(fd, tam, &bloqueado, logs);
					recibirDatos(fd, tam, (void*) pcb, logs);
					ponerCola(pcb, tSem->cola, &tSem->mutex, &tSem->hayAlgo);
					muestraNombres(tSem->cola,"WAIT BLOQUEADOS");
					sem_post(&mutexSemaforos);
					PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //mando de nuevo.
					muestraNombres(READY,"READY ");
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
					bloqueado = 0;
					enviarDatos(fd, tam, &bloqueado, logs);
					log_info(logs, "y su valor es %i", tSem->valor);
					sem_post(&mutexSemaforos);
				}

				break;
			case SIGNAL:

				log_info(logs, "SYSCALL: SIGNAL");
				recibirDato(fd, tam->length, (void*) &nombreSem, logs);
				sem = string_from_format("%s", &nombreSem);

				log_info(logs, "El semaforo es %s", sem);
				sem_wait(&mutexSemaforos);
				tSem = dictionary_get(semaforos, sem);
				tSem->valor = tSem->valor + 1;
				if (!queue_is_empty(tSem->cola)) {

					PCBPOP = sacarCola(tSem->cola, &tSem->mutex, &tSem->hayAlgo);
					muestraNombres(tSem->cola,"SIGNAL BLOQUEADOS");
					ULTIMOPCB = PCBPOP;

					printf("voya poner en ready el PID IMPORTANTE");

					ponerCola(PCBPOP, READY, &mutexREADY, &hayAlgoEnReady);
					muestraNombres(READY,"READY");

				}
				log_info(logs, "y su valor es %i", tSem->valor);
				sem_post(&mutexSemaforos);

				break;
			case CONCLUYO_UN_QUANTUM:

				recibirDato(fd, tam->length, (void*) pcb, logs);

				ponerCola(pcb, READY, &mutexREADY, &hayAlgoEnReady); //lo pongo en ready
				muestraNombres(READY,"READY");
				log_info(logs, "Se coloco en la cola Ready el proceso %i",
						pcb->pid);
				log_debug(logss, "El FD es %d", fd);
				if (fd >= 0){
					PCBPOP = sacarCola(READY, &mutexREADY, &hayAlgoEnReady); //saco de ready
					muestraNombres(READY,"QUATUM READY ");
					ULTIMOPCB = PCBPOP;
					log_info(logs, "Se saco de la cola Ready el proceso %i", PCBPOP->pid);
					sem_wait(&mutexMandarColaEXEC);	//envio datos y pongo en exec atomicamente
					ponerCola(PCBPOP, EXEC, &mutexEXEC, &hayAlgoEnExec);
					log_info(logs, "Se coloco en la cola EXEC el proceso %i", PCBPOP->pid);
					tam->length = sizeof(registroPCB);
					enviarDatos(fd, tam, PCBPOP, logs);
					sem_post(&mutexMandarColaEXEC);
				}
				break;

			case FINALIZAR:
				recibirDato(fd, tam->length, (void*) PCBrecibido, logs);
				//sem_wait(&mutexEXEC);
				//EXEC = list_filter(EXEC, (void*)condicion); //saco de la cola exec

				//sem_post(&mutexEXEC);

				ponerCola(PCBrecibido, EXIT, &mutexEXIT, &hayAlgoEnExit); //la pongo en exit
				sem_post(&gradoProg);
				//eliminarSegmentoUMV(socket_UMV, logs, PCBrecibido);//FIXME: haciendo esto los eliminas dos veces, porque tambien lo hace manejoExit()

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

			/*case ERROR:
				ponerCola(ULTIMOPCB,EXIT, &mutexEXIT, &hayAlgoEnExit);
				char* error = string_from_format("%d", ULTIMOPCB->pid);
				int* fdError= dictionary_get(fileDescriptors, error);
				enviarMenu(*fdError, tam, logs);
				log_error(logs, "Se llamo a la senial SIGUSR1 en la CPU y finalizo su ejecucion");
				pthread_exit(1);
				break;*/
			default:

				ponerCola(ULTIMOPCB,EXIT,&mutexEXIT,&hayAlgoEnExit);
				char* p2 = string_from_format("%d", ULTIMOPCB->pid);
				int* fd2= dictionary_get(fileDescriptors, p2);

				tam->menu = ERROR;
				enviarMenu(*fd2, tam, logs);
				log_error(logs,
						"Se interrumpio el proceso con el PID: %d debido a que la CPU esta caida",
						ULTIMOPCB->pid);

				log_error(logs, "Se cierra la CPU.");
				pthread_exit(1);

				break;
				}
			}

}

