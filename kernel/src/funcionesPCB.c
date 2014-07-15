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
int identificadorUnico = 1;
extern int tamanioStack;

void intercambiarDatosUMV(int socket_UMV, t_log* logs, registroPCB* PCBprograma, char* buf){

	t_length *tam;
	tam = malloc(sizeof(t_length));
//	tam->menu = SOY_KERNEL;
//	log_debug(logs,"Entra al while para enviar menu");
//	//while(1){
//		if(!enviarMenu(socket_UMV, tam, logs))//Le aviso a la UMV que soy el kernel
//			log_error(logs,"Error en la identificacion ... (LA UMV NO RECIBE AL KERNEL)");
//	//	else
//	//		break;
////	}
//	log_debug(logs,"ya se envio el menu");
/*	tam->menu = PID_ACTUAL ;
	int datos_pid = PCBprograma.pid;								/////SACAMOS ESTO???////
	if(!enviarDatos(socket_UMV, tam, (void*)datos_pid,logs))
			log_error(logs,"Error en el envio del pid");*/

	datos_crearSeg* datosAEnviar=malloc(sizeof(datos_crearSeg));
	tam->length = sizeof(datos_crearSeg);
	t_metadata_program* metadataP = metadata_desde_literal(buf);

	int datoRecibido;
//////////////////////////////SEGMENTO INSTRUCCIONES /////////////////////////////////////////////////
	datosAEnviar->pid=PCBprograma->pid;
		log_info(logs,"%i",datosAEnviar->pid);
		datosAEnviar->tamanio = metadataP->instrucciones_size;
		tam->menu = CREAR_SEGMENTO;
		tam->length = sizeof(datos_crearSeg);

		if(!enviarDatos(socket_UMV, tam, datosAEnviar, logs)){
			log_error( logs ,"Error en el envio de los datos...(FALLO EN EL ENVIO DEL SEGMENTO ETIQUETAS_FUNCIONES)");
			exit(EXIT_FAILURE);
		}
		log_debug(logs,"se envio el crear_seg Instrucciones");
		if (!recibirDatos (socket_UMV,tam,(void*)&datoRecibido,logs)){ //base
			log_error (logs,"Error en el envio del Segmento Instrucciones");
			exit(EXIT_FAILURE);
		}else{
			if(datoRecibido != -1)
				PCBprograma->indice_codigo = datoRecibido;  //YA TENGO LA BASE INDICE CODIGO
			else{
				log_error (logs, "La UMV se quedo sin memoria");
				log_error (logs,"Se ha abortado el proceso de CREACION DE SEGMENTOS");
				exit(EXIT_FAILURE);
			}
		}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//envio segmento etiquetas_funciones
	datosAEnviar->pid=PCBprograma->pid;
	log_info(logs,"%i",datosAEnviar->pid);
	datosAEnviar->tamanio = PCBprograma->tamanio_indice_etiquetas;
	tam->menu = CREAR_SEGMENTO;
	tam->length = sizeof(datos_crearSeg);

	if(!enviarDatos(socket_UMV, tam, datosAEnviar, logs)){
		log_error( logs ,"Error en el envio de los datos...(FALLO EN EL ENVIO DEL SEGMENTO ETIQUETAS_FUNCIONES)");
		exit(EXIT_FAILURE);
	}
	log_debug(logs,"se envio el crear_seg et");
	if (!recibirDatos (socket_UMV,tam,(void*)&datoRecibido,logs)){ //base
		log_error (logs,"Error en el envio del Segmento Etiquetas_Funciones");
		exit(EXIT_FAILURE);
	}else{
		if(datoRecibido != -1)
			PCBprograma->indice_etiquetas = datoRecibido;  //YA TENGO LA BASE INDICE ETIQUETAS
		else{
			log_error (logs, "La UMV se quedo sin memoria");
			log_error (logs,"Se ha abortado el proceso de CREACION DE SEGMENTOS");
			exit(EXIT_FAILURE);
		}
	}


	//envio segmento etiquetas
	datosAEnviar->tamanio = PCBprograma->tamanio_indice_etiquetas;
	tam->menu = CREAR_SEGMENTO;
	tam->length = sizeof(datos_crearSeg);
	if(!enviarDatos(socket_UMV, tam, datosAEnviar, logs))
		log_error( logs ,"Error en el envio de los datos");
	log_debug(logs,"se envio el crear_seg tam_ind_cod");
	if (!recibirDatos (socket_UMV,tam,(void*)&datoRecibido,logs))
		log_error (logs,"Error en el envio del Segmento Indice_Codigo");
	else{
		if(datoRecibido >= 0){
			PCBprograma->indice_etiquetas = datoRecibido; //BASE DEL SEGMENTO ETIQUETAS
			log_debug(logs,"se recibio la base del segmento codigo");
		}else{
			log_error (logs, "La UMV se quedo sin memoria");
			log_error (logs,"Se ha abortado el proceso de CREACION DE SEGMENTOS");
			exit(EXIT_FAILURE);
		}
	}


//ESTO NO SE COMO HACER... FALTA ALGO?? ¿HAY QUE VOLVER A ENVIAR?


	//envio stack
	datosAEnviar->tamanio = tamanioStack;
	tam->menu = CREAR_SEGMENTO;
	tam->length = sizeof(datos_crearSeg);
	if(!enviarDatos(socket_UMV, tam, datosAEnviar, logs))
		log_error(logs,"Error en el envio de los datos");
	log_debug(logs,"se envio el crear_seg stack");
//
	if(!recibirDatos(socket_UMV,tam,(void*)&datoRecibido,logs))
		log_error (logs, "Error en el recibir del segmento Stack");
	else{
		if(datoRecibido >= 0)
			PCBprograma->segmento_stack = datoRecibido;
		else{
			log_error(logs,"La UMV se quedo sin memoria");
			log_error (logs,"Se ha abortado el proceso de CREACION DE SEGMENTOS");
			exit(EXIT_FAILURE);
		}
		log_debug(logs,"se envio el recibio el segmento stack");
	}

	//envio instrucciones serializadas
	datosAEnviar->tamanio = PCBprograma->tamanio_indice_codigo;
	tam->menu = CREAR_SEGMENTO;
	tam->length = sizeof(datos_crearSeg);
	if(!enviarDatos(socket_UMV, tam, datosAEnviar, logs))
		log_error(logs,"Error en el envio de los datos");
	log_debug(logs,"se envio el crear_seg codigo");
//
	if(!recibirDatos(socket_UMV,tam,(void*)&datoRecibido,logs))
		log_error (logs, "Error en el recibir del segmento Codigo");
	else{
		if(datoRecibido >= 0)
			PCBprograma->indice_codigo = datoRecibido; //base del segmento de instrucciones
		else{
			log_error(logs,"La UMV se quedo sin memoria");
			log_error (logs,"Se ha abortado el proceso de CREACION DE SEGMENTOS");
			exit(EXIT_FAILURE);
		}
		log_debug(logs,"se envio el recibio el segmento codigo");
	}


	//envio el codigo
//	tam->menu= PID_ACTUAL;
//	tam->length = sizeof(int);
	int pid = PCBprograma->pid;
//	if(!enviarDatos(socket_UMV, tam, &pid,logs))
//		log_error(logs,"Error en el envio de los datos");
//
//	tam->menu = ESCRIBIR_SEGMENTO;
	t_etiqueta *etiq=malloc(sizeof(t_etiqueta));
//	etiq->base=PCBprograma->indice_codigo;
//	etiq->offset=0;
//	etiq->tamanio=PCBprograma->tamanio_indice_codigo;
//	tam->length = sizeof(t_etiqueta);
//
//	if(!enviarDatos(socket_UMV, tam, etiq,logs))
//		log_error(logs,"Error en el envio de los datos");
//
//	tam->length = PCBprograma->segmento_codigo;
//
//	if(!enviarDatos(socket_UMV, tam, buf,logs))
//		log_error(logs,"Error en el envio de los datos");
//
//	log_debug(logs,"se envio el escribir segmento codigo");
//


///////////////////////////ESCRIBIR INSTRUCCIONES//////////////////////////////

	tam->menu= PID_ACTUAL;
	tam->length = sizeof(int);
	pid = PCBprograma->pid;
	if(!enviarDatos(socket_UMV, tam, &pid,logs))
		log_error(logs,"Error en el envio de los datos");

	tam->menu = ESCRIBIR_SEGMENTO;
	etiq->base=PCBprograma->indice_codigo;
	etiq->offset=0;
	etiq->tamanio=PCBprograma->tamanio_indice_codigo;
	tam->length = sizeof(t_etiqueta);

	if(!enviarDatos(socket_UMV, tam, etiq,logs))
		log_error(logs,"Error en el envio de los datos");

	tam->length = PCBprograma->segmento_codigo;

	if(!enviarDatos(socket_UMV, tam, metadataP->instrucciones_serializado ,logs))
		log_error(logs,"Error en el envio de los datos");

	log_debug(logs,"se envio el escribir segmento instrucciones");


log_info(logs,"Se termino el proceso de CONEXION y CREACION DE SEGMENTOS");
}


//////////////////////////////////////////////////////////////////////////////
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
	if(!enviarDatos(socket_UMV, tam, &datos_pid,logs))
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
	unPCB->fd=fd;
	unPCB->peso = peso;
	unPCB->puntero_etiquetas = 0 ;
	unPCB->cursor_stack = 0;
	unPCB->tamanio_indice_etiquetas = metadataP->etiquetas_size;
    unPCB->pid = identificadorUnico;
    unPCB->tamanio_contexto= 0;
    unPCB->indice_codigo = 0 ; //se empieza en 0 por ser base ??
    unPCB->tamanio_indice_codigo=metadataP->instrucciones_size;
    identificadorUnico = identificadorUnico + 1;
    log_info(logs,"se creo el pcb. entra a intercambiar datos con  la UMV");
    intercambiarDatosUMV(socket_UMV,logs,unPCB, program);// ESTO NO LO EJECUTO HASTA QUE NO PUEDA CONECTARSE DE FORMA EXITOSA
    													//CON LA UMV.

    return unPCB;
}





