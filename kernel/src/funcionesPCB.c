/*
 * manejoDelPCB.c
 *
 *  Created on: 24/05/2014
 *      Author: utnso
 */


#include <libs/manejoDelPCB.h>
#include "planificador_largo_plazo.h"
#include <commons/log.h>
#include "funcionesPCB.h"


extern t_dictionary * fileDescriptors;
extern int socket_UMV;
extern t_config* config;
extern t_log* logs;
int identificadorUnico = 1;
extern int tamanioStack;


void conectarseUMV(){
    char *ip = config_get_string_value(config,"IP");
    int port = config_get_int_value(config,"PUERTO_UMV");
	log_debug(logs,"el puerto es: %d", port);
    socket_UMV = conectarCliente(ip, port, logs);
	if(socket_UMV < 0){
		log_error(logs, "El cliente no se pudo conectar correctamente con la UMV");
		log_destroy(logs);
		exit(EXIT_FAILURE);
	}else
		log_info(logs, "El kernel se conecto correctamente con la UMV");
	t_length *tam;
		tam = malloc(sizeof(t_length));
		tam->menu = SOY_KERNEL;

			if(!enviarMenu(socket_UMV, tam, logs))//Le aviso a la UMV que soy el kernel
				log_error(logs,"Error en la identificacion ... (LA UMV NO RECIBE AL KERNEL)");

		log_debug(logs,"ya se envio el menu");
}
// El socket de la UMV habria que cerrarlo una vez que se resfieren los datos y todo...

void eliminarSegmentoUMV(int socket_UMV, t_log* logs,registroPCB* PCBprograma){
	t_length *tam;
	tam = malloc(sizeof(t_length));
	tam->menu = ELIMINAR_SEGMENTOS;
	tam->length = sizeof(int);
	int datos_pid = PCBprograma->pid;
	if(!enviarDatos(socket_UMV, tam, &datos_pid,logs))
		log_error(logs,"Error en el envio del pid, no se pudo DESTRUIR EL SEGMENTO");
	else
		log_info (logs, "Se pidio la destruccion del SEGMENTO de forma EXITOSA relacionada al proceso: %d",datos_pid);
}


int algoritmoDePeso(int cantEtq, int cantfc, t_size cantInstruc){

	int peso = 5*cantEtq+ 3*cantfc + cantInstruc;
	return peso;
}

/* crea un segmento y retorna la base*/
int crearSegmento(registroPCB* PCBprograma,t_log* logs , int tamanio, int socket_UMV){
	t_length *tam;
	tam = malloc(sizeof(t_length));
	datos_crearSeg* datosAEnviar=malloc(sizeof(datos_crearSeg));
	tam->length = sizeof(datos_crearSeg);


	int datoRecibido;

	datosAEnviar->pid=PCBprograma->pid;
	log_info(logs,"%i",datosAEnviar->pid);
	datosAEnviar->tamanio = tamanio;
	tam->menu = CREAR_SEGMENTO;
	tam->length = sizeof(datos_crearSeg);

			if(!enviarDatos(socket_UMV, tam, datosAEnviar, logs)){
				log_error( logs ,"Error en el envio de los datos...(FALLO EN EL ENVIO DEL SEGMENTO)");
				exit(EXIT_FAILURE);
			}
			log_debug(logs,"se envio el Crear Segmento");
			if (!recibirDatos (socket_UMV,tam,(void*)&datoRecibido,logs)){ //base
				log_error (logs,"Error en el envio del Segmento ");
				exit(EXIT_FAILURE);
			}else{
				if(datoRecibido != -1)
					return datoRecibido;  //BASE
				else{
					log_error (logs, "La UMV se quedo sin memoria");
					log_error (logs,"Se ha abortado el proceso de CREACION DE SEGMENTOS");
					exit(EXIT_FAILURE);
				}
			}
}

/*escribe un segmento*/
void escribirSegmento(registroPCB* PCBprograma, int base ,int tamanio, void* datoAEnviar){
	t_length* tam = malloc(sizeof(t_length));
	t_etiqueta *etiq=malloc(sizeof(t_etiqueta));
	int pid;

	tam->menu= PID_ACTUAL;
	tam->length = sizeof(int);
	pid = PCBprograma->pid;
	if(!enviarDatos(socket_UMV, tam, &pid,logs))
		log_error(logs,"Error en el envio de los datos");

	tam->menu = ESCRIBIR_SEGMENTO;
	etiq->base=base;
	etiq->offset=0;
	etiq->tamanio=tamanio;
	tam->length = sizeof(t_etiqueta);

	if(!enviarDatos(socket_UMV, tam, etiq,logs))
		log_error(logs,"Error en el envio del tamaño de los datos");

	tam->length = tamanio;
//	t_aux* aux;
//	aux = datoAEnviar;
//	log_debug(logs, "Offset: %d; Tamanio: %d", aux->offset, aux->tamanio);
	if(!enviarDatos(socket_UMV, tam, datoAEnviar ,logs))
		log_error(logs,"Error en el envio de los datos");

	log_debug(logs,"se envio el escribir segmento");


	log_info(logs,"Se termino el proceso de escritura");
}

registroPCB* armarPCB(char* program, int fd){

  	log_debug(logs,"entra a metadata desde literal");
	t_metadata_program* metadataP = metadata_desde_literal(program);
	log_debug(logs,"sale de metadata desde literal");
	log_debug(logs,"entra a algoritmo de peso");
	int peso =algoritmoDePeso(metadataP->cantidad_de_etiquetas,metadataP->cantidad_de_funciones,metadataP->instrucciones_size);
	log_debug(logs,"sale de algoritmo de peso");

	registroPCB* unPCB;

	unPCB=malloc(sizeof(registroPCB));

	char * keyPID= string_from_format("%d", identificadorUnico);
	int* f = malloc(sizeof(int));
	*f = fd;
	dictionary_put(fileDescriptors,keyPID,f);

	unPCB->program_counter=metadataP->instruccion_inicio;
	unPCB->peso = peso;
	unPCB->puntero_etiquetas = 0 ;
	unPCB->cursor_stack = 0;
	unPCB->tamanio_indice_etiquetas = metadataP->etiquetas_size;
    unPCB->pid = identificadorUnico;
    unPCB->tamanio_contexto= 0;
    unPCB->cursor_anterior = 0;
    unPCB->tamanio_indice_codigo=metadataP->instrucciones_size * sizeof(t_intructions);
    unPCB->indice_etiquetas = crearSegmento(unPCB,logs,unPCB->tamanio_indice_etiquetas,socket_UMV);
    unPCB->segmento_stack = crearSegmento(unPCB,logs,tamanioStack,socket_UMV);
    unPCB->segmento_codigo = crearSegmento(unPCB,logs,(strlen(program)+1),socket_UMV);
    unPCB->indice_codigo = crearSegmento(unPCB,logs,unPCB->tamanio_indice_codigo,socket_UMV);

    identificadorUnico = identificadorUnico + 1;

    escribirSegmento(unPCB,unPCB->indice_codigo,unPCB->tamanio_indice_codigo,metadataP->instrucciones_serializado);

    if(unPCB->tamanio_indice_etiquetas > 0)
    	escribirSegmento(unPCB,unPCB->indice_etiquetas,unPCB->tamanio_indice_etiquetas,metadataP->etiquetas);
    printf("SE ENVIA ESCRIBIR %d", unPCB->segmento_codigo);
    escribirSegmento(unPCB,unPCB->segmento_codigo,(strlen(program)+1),program);

    usleep(32);

    log_info(logs,"se creo el pcb. entra a intercambiar datos con  la UMV");

    return unPCB;
}





