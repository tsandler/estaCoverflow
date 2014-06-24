/*
 * planificador_largo_plazo.c
 *
 *  Created on: 18/06/2014
 *      Author: utnso
 */
#include "globales.h"

extern t_queue *NEW;
extern t_queue *READY;
extern sem_t mutexNEW;
extern sem_t mutexREADY;
extern sem_t gradoProg;
extern sem_t hayAlgo;
extern sem_t hayAlgoEnReady;
extern t_log *logs;
extern t_config *config;

/*
 *
 * saca el pcb de peso menor. para el SNF
 *
 */

registroPCB *queue_pop_min(t_queue *self) {
	int tamanio=queue_size(self);
	registroPCB* minPCB = queue_pop(self);

	log_info(logs,"Se saco el programa seg{un SJN");
	int i=1;
	while(i<tamanio){
		registroPCB* otroPCB = queue_pop(self);
	if(otroPCB->peso > minPCB->peso){
		queue_push(self,otroPCB);
	}
	else {
		queue_push(self,minPCB);
		minPCB=otroPCB;
	}
	i=i+1;
	}
	return minPCB;


}


void deNewAReady(void){
	while(1){

			sem_wait(&hayAlgo);
			sem_wait(&gradoProg);
			sem_wait(&mutexNEW);

			int value;

			sem_getvalue(&gradoProg,&value);
			log_info(logs, "El grado de multiprogramacion es : %i \n",value);
			log_info(logs,"En la cola New hay: %i \n",queue_size(NEW));
			registroPCB* unPCB=queue_pop_min(NEW);
			log_info(logs,"El pop saco el peso %d \n", unPCB->peso);
			log_info(logs,"En la cola NEW hay %d \n",queue_size(NEW));
			sem_post(&mutexNEW);

			sem_wait(&mutexREADY);

			log_info(logs,"En la cola Ready Hay %i \n", queue_size(READY));
			queue_push(READY,unPCB);
			log_info(logs,"En la cola Ready Hay %d \n",queue_size(READY));
			sem_post(&hayAlgoEnReady);
			sem_post(&mutexREADY);

	}

}


void plp(void* ptr){

	pthread_t thread4;
	pthread_t thread5;
	int port=config_get_int_value(config, "PUERTO_PROG");




	int iret1 = pthread_create(&thread4, NULL, openSocketServerPLP,(void*)port);

		if (iret1){
			log_info(logs, "Error en la creacion del hilo openSocketServerPLP");
			exit(EXIT_FAILURE);
		}

	int iret2 = pthread_create(&thread5, NULL, deNewAReady,(void*)port);

		if (iret2){
			log_info(logs,"Error en la creacion del hilo deNewAReady");
			exit (EXIT_FAILURE);
		}
		pthread_join(thread4, NULL ); //esperar a q termine el otro

		pthread_join(thread5, NULL );

}

void intercambiarDatosUMV(int socket_UMV, t_log* logs, registroPCB PCBprograma){

	t_length *tam;
	tam = malloc(sizeof(t_length));
	tam->menu = SOY_KERNEL;
	while(1){
	if(!enviarMenu(socket_UMV, tam, logs))//Le aviso a la UMV que soy el kernel
		log_error(logs,"Error en la identificacion");
	else
		break;
	}
	tam->menu = PID_ACTUAL ;
	int datos_pid = PCBprograma.pid;
	if(!enviarDatos(socket_UMV, tam, (void*)datos_pid,logs))
			log_error(logs,"Error en el envio del pid");

	datos_crearSeg* datosAEnviar;
	tam->length = sizeof(datos_crearSeg);

	int datoRecibido;

	//envio segmento etiquetas_funciones
	datosAEnviar->tamanio = PCBprograma.tamanio_indice_etiquetas_funciones;
	tam->menu = CREAR_SEGMENTO;
	if(!enviarDatos(socket_UMV, tam, (void*)datosAEnviar, logs))
		log_error( logs ,"Error en el envio de los datos");
	if (!recibirDatos (socket_UMV,tam,(void*)datoRecibido,logs))
		log_error (logs,"Error en el envio del Segmento Etiquetas_Funciones");
	else{
		if(datoRecibido > 0)
			PCBprograma.puntero_etiquetas_funciones = datoRecibido;
		else{
			log_error (logs, "La UMV se quedo sin memoria");
			goto termino_ejecucion;
		}
	}


	//envio segmento Codigo
	datosAEnviar->tamanio = PCBprograma.tamanio_indice_codigo;
	tam->menu = CREAR_SEGMENTO;
	if(!enviarDatos(socket_UMV, tam, (void*)datosAEnviar, logs))
		log_error( logs ,"Error en el envio de los datos");
	if (!recibirDatos (socket_UMV,tam,(void*)datoRecibido,logs))
		log_error (logs,"Error en el envio del Segmento Indice_Codigo");
	else{
		if(datoRecibido > 0)
			PCBprograma.segmento_codigo = datoRecibido;
		else{
			log_error (logs, "La UMV se quedo sin memoria");
			goto termino_ejecucion;
		}
	}


	//envio contexto
	datosAEnviar->tamanio = PCBprograma.tamanio_contexto;
	tam->menu = CREAR_SEGMENTO;
	if(!enviarDatos(socket_UMV, tam, (void*)datosAEnviar,logs))
		log_error(logs,"Error en el envio de los datos");
	if(!recibirDatos(socket_UMV,tam,(void*)datoRecibido,logs))
		log_error (logs, "Error en el envio del Contexto");
	else{
		if(datoRecibido > 0)
			PCBprograma.program_counter = datoRecibido; //No estoy seguro si tengo que asignar esto lo podriamos charlar...habria que cambiar algo del PCB sino.
		else{
			log_error(logs,"La UMV se quedo sin memoria");
			goto termino_ejecucion;
		}
	}

	//envio stack
	datosAEnviar->tamanio = config_get_int_value(config,"TAMANIO_STACK");
	tam->menu = CREAR_SEGMENTO;
	if(!enviarDatos(socket_UMV, tam, (void*)datosAEnviar, logs))
		log_error(logs,"Error en el envio de los datos");
	if(!recibirDatos(socket_UMV,tam,(void*)datoRecibido,logs))
		log_error (logs, "Error en el envio del segmento Stack");
	else{
		if(datoRecibido > 0)
			PCBprograma.segmento_stack = datoRecibido;
		else{
			log_error(logs,"La UMV se quedo sin memoria");
			goto termino_ejecucion;
		}
	goto termino;
	}
termino_ejecucion:
log_error (logs,"Se ha abortado el proceso de CREACION DE SEGMENTOS");

termino:
log_info(logs,"Se termino el proceso de CONEXION y CREACION DE SEGMENTOS");
}

void conectarseUMV(registroPCB PCBprograma, t_log *logs){
    int socket_UMV;
    char *ip = config_get_string_value(config,"IP");
    int port = config_get_int_value(config,"PUERTO_UMV");
    socket_UMV = conectarCliente(ip, port, logs);
	if(socket_UMV < 0){
		log_error(logs, "El cliente no se pudo conectar correctamente");
		log_destroy(logs);
	}else{
		log_info(logs, "El kernel se conecto correctamente con la UMV");
		intercambiarDatosUMV(socket_UMV,logs,PCBprograma);
	}
}
// El socket de la uMV habria que cerrarlo una vez que se resfieren los datos y todo...

void eliminarSegmentoUMV(int socket_UMV, t_log* logs,registroPCB PCBprograma){
	t_length *tam;
	tam = malloc(sizeof(t_length));
	tam->menu = DESTRUIR_SEGMENTO;
	while(1){
		if(!enviarMenu(socket_UMV, tam, logs))//Le aviso a la UMV que soy el kernel
			log_error(logs,"Error en la identificacion");
		else
			break;
	}
	tam->menu = PID_ACTUAL ;
	int datos_pid = PCBprograma.pid;
	if(!enviarDatos(socket_UMV, tam, (void*)datos_pid,logs))
		log_error(logs,"Error en el envio del pid");
	else
		log_info (logs, "Se pidio la destruccion del SEGMENTO de forma EXITOSA");
}
