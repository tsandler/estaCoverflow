/*
 * manejoDelPCB.c
 *
 *  Created on: 24/05/2014
 *      Author: utnso
 */


#include <libs/manejoDelPCB.h>
#include "planificador_largo_plazo.h"
#include <commons/log.h>

extern int socket_UMV;
extern t_config* config;
extern t_log* logs;
int identificadorUnico = 0;



void intercambiarDatosUMV(int socket_UMV, t_log* logs, registroPCB* PCBprograma, char* buf){

	t_length *tam;
	tam = malloc(sizeof(t_length));
	tam->menu = SOY_KERNEL;
	log_debug(logs,"Entra al while para enviar menu");
	//while(1){
		if(!enviarMenu(socket_UMV, tam, logs))//Le aviso a la UMV que soy el kernel
			log_error(logs,"Error en la identificacion ... (LA UMV NO RECIBE AL KERNEL)");
	//	else
	//		break;
//	}
	log_debug(logs,"ya se envio el menu");
/*	tam->menu = PID_ACTUAL ;
	int datos_pid = PCBprograma.pid;								/////SACAMOS ESTO???////
	if(!enviarDatos(socket_UMV, tam, (void*)datos_pid,logs))
			log_error(logs,"Error en el envio del pid");*/

	datos_crearSeg* datosAEnviar=malloc(sizeof(datos_crearSeg));
	tam->length = sizeof(datos_crearSeg);

	int datoRecibido;

	//envio segmento etiquetas_funciones
	datosAEnviar->pid=PCBprograma->pid;
	datosAEnviar->tamanio = PCBprograma->tamanio_indice_etiquetas_funciones;
	tam->menu = CREAR_SEGMENTO;
	if(!enviarDatos(socket_UMV, tam, (void*)datosAEnviar, logs)){
		log_error( logs ,"Error en el envio de los datos...(FALLO EN EL ENVIO DEL SEGMENTO ETIQUETAS_FUNCIONES)");
		exit(EXIT_FAILURE);
	}
	if (!recibirDatos (socket_UMV,tam,(void*)datoRecibido,logs)){ //base
		log_error (logs,"Error en el envio del Segmento Etiquetas_Funciones");
		exit(EXIT_FAILURE);
	}else{
		if(datoRecibido >= 0)
			PCBprograma->puntero_etiquetas_funciones = datoRecibido;  //YA TENGO LA BASE INDICE ETIQUETAS
		else{
			log_error (logs, "La UMV se quedo sin memoria");
			log_error (logs,"Se ha abortado el proceso de CREACION DE SEGMENTOS");
			exit(EXIT_FAILURE);
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
			log_error (logs,"Se ha abortado el proceso de CREACION DE SEGMENTOS");
			exit(EXIT_FAILURE);
		}
	}


//ESTO NO SE COMO HACER... FALTA ALGO?? ¿HAY QUE VOLVER A ENVIAR?
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
			log_error (logs,"Se ha abortado el proceso de CREACION DE SEGMENTOS");
			exit(EXIT_FAILURE);
		}
	}

log_info(logs,"Se termino el proceso de CONEXION y CREACION DE SEGMENTOS");
}

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
	while(1){
		if(!enviarMenu(socket_UMV, tam, logs))//Le aviso a la UMV que soy el kernel
			log_error(logs,"Error en la identificacion");
		else
			break;
	}
	tam->menu = PID_ACTUAL ;
	int datos_pid = PCBprograma->pid;
	if(!enviarDatos(socket_UMV, tam, (void*)datos_pid,logs))
		log_error(logs,"Error en el envio del pid, no se pudo DESTRUIR EL SEGMENTO");
	else
		log_info (logs, "Se pidio la destruccion del SEGMENTO de forma EXITOSA");
}


int algoritmoDePeso(int cantEtq, int cantfc, t_size cantInstruc){

	int peso = 5*cantEtq+ 3*cantfc + cantInstruc;
	return peso;
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


	unPCB->program_counter=metadataP->instruccion_inicio;
	unPCB->tamanio_indice_etiquetas_funciones=metadataP->etiquetas_size;
	unPCB->tamanio_indice_codigo=metadataP->instrucciones_size;
	unPCB->fd=fd;
	unPCB->peso = peso;
	identificadorUnico =+ 1;
    unPCB->pid = identificadorUnico;

    log_info(logs,"se creo el pcb. entra a intercambiar datos con la UMV");
    intercambiarDatosUMV(socket_UMV,logs,unPCB, program);// ESTO NO LO EJECUTO HASTA QUE NO PUEDA CONECTARSE DE FORMA EXITOSA
	return unPCB;                                        //CON LA UMV.
}


