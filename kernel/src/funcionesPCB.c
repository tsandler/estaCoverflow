/*
 * manejoDelPCB.c
 *
 *  Created on: 24/05/2014
 *      Author: utnso
 */


#include <libs/manejoDelPCB.h>
#include "planificador_largo_plazo.h"
#include <commons/log.h>

extern int socketUMV;
extern t_config* config;
extern t_log* logs;
int identificadorUnico = 0;



void intercambiarDatosUMV(int socket_UMV, t_log* logs, registroPCB* PCBprograma, char* buf){

	t_length *tam;
	tam = malloc(sizeof(t_length));
	tam->menu = SOY_KERNEL;
	while(1){
	if(!enviarMenu(socket_UMV, tam, logs))//Le aviso a la UMV que soy el kernel
		log_error(logs,"Error en la identificacion");
	else
		break;
	}
/*	tam->menu = PID_ACTUAL ;
	int datos_pid = PCBprograma.pid;
	if(!enviarDatos(socket_UMV, tam, (void*)datos_pid,logs))
			log_error(logs,"Error en el envio del pid");*/

	datos_crearSeg* datosAEnviar;

	tam->length = sizeof(datos_crearSeg);

	int datoRecibido;

	//envio segmento etiquetas_funciones
	datosAEnviar->pid=PCBprograma->pid;
	datosAEnviar->tamanio = PCBprograma->tamanio_indice_etiquetas_funciones;
	tam->menu = CREAR_SEGMENTO;
	if(!enviarDatos(socket_UMV, tam, (void*)datosAEnviar, logs))
		log_error( logs ,"Error en el envio de los datos");
	if (!recibirDatos (socket_UMV,tam,(void*)datoRecibido,logs)) //base
		log_error (logs,"Error en el envio del Segmento Etiquetas_Funciones");
	else{
		if(datoRecibido >= 0)
			PCBprograma->puntero_etiquetas_funciones = datoRecibido;  //YA TENGO LA BASE INDICE ETIQUETAS
		else{
			log_error (logs, "La UMV se quedo sin memoria");
			goto termino_ejecucion;
		}
	}


	//envio segmento Codigo
	datosAEnviar->tamanio = PCBprograma->tamanio_indice_codigo;
	tam->menu = CREAR_SEGMENTO;
	if(!enviarDatos(socket_UMV, tam, (void*)datosAEnviar, logs))
		log_error( logs ,"Error en el envio de los datos");
	if (!recibirDatos (socket_UMV,tam,(void*)datoRecibido,logs))
		log_error (logs,"Error en el envio del Segmento Indice_Codigo");
	else{
		if(datoRecibido >= 0)
			PCBprograma->segmento_codigo = datoRecibido; //BASE DEL SEGMENTO CODIGO
		else{
			log_error (logs, "La UMV se quedo sin memoria");
			goto termino_ejecucion;
		}
	}



	//envio el codigo entero
		datosAEnviar->tamanio = strlen(buf)+1;
		tam->menu = ESCRIBIR_SEGMENTO;
		if(!enviarDatos(socket_UMV, tam, (void*)datosAEnviar,logs))
			log_error(logs,"Error en el envio de los datos");
		if(!enviarDatos(socket_UMV, tam, buf,logs))
					log_error(logs,"Error en el envio de los datos");

	//envio stack
	datosAEnviar->tamanio = config_get_int_value(config,"TAMANIO_STACK");
	tam->menu = CREAR_SEGMENTO;
	if(!enviarDatos(socket_UMV, tam, (void*)datosAEnviar, logs))
		log_error(logs,"Error en el envio de los datos");
	if(!recibirDatos(socket_UMV,tam,(void*)datoRecibido,logs))
		log_error (logs, "Error en el envio del segmento Stack");
	else{
		if(datoRecibido >= 0)
			PCBprograma->segmento_stack = datoRecibido;
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

int conectarseUMV(){
    int socket_UMV;
    char *ip = config_get_string_value(config,"IP");
    int port = config_get_int_value(config,"PUERTO_UMV");
    socket_UMV = conectarCliente(ip, port, logs);
	if(socket_UMV < 0){
		log_error(logs, "El cliente no se pudo conectar correctamente");
		log_destroy(logs);
	}else{
		log_info(logs, "El kernel se conecto correctamente con la UMV");

		return socket_UMV;
	}
}
// El socket de la uMV habria que cerrarlo una vez que se resfieren los datos y todo...

void eliminarSegmentoUMV(int socket_UMV, t_log* logs,registroPCB* PCBprograma){
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
	int datos_pid = PCBprograma->pid;
	if(!enviarDatos(socket_UMV, tam, (void*)datos_pid,logs))
		log_error(logs,"Error en el envio del pid");
	else
		log_info (logs, "Se pidio la destruccion del SEGMENTO de forma EXITOSA");
}


int algoritmoDePeso(int cantEtq, int cantfc, t_size cantInstruc){

	int peso = 5*cantEtq+ 3*cantfc + cantInstruc;
	return peso;
}


registroPCB* armarPCB(char* program, int fd){


	t_metadata_program* metadataP = metadata_desde_literal(program);

	int peso =algoritmoDePeso(metadataP->cantidad_de_etiquetas,metadataP->cantidad_de_funciones,metadataP->instrucciones_size);

	registroPCB* unPCB;

	unPCB=malloc(sizeof(registroPCB));


	unPCB->program_counter=metadataP->instruccion_inicio;
	unPCB->tamanio_indice_etiquetas_funciones=metadataP->etiquetas_size;
	unPCB->tamanio_indice_codigo=metadataP->instrucciones_size;
	unPCB->fd=fd;
	unPCB->peso = peso;
	identificadorUnico =+ 1;
    unPCB->pid = identificadorUnico;


  // intercambiarDatosUMV(socketUMV,logs,unPCB, program);
	return unPCB;


}


