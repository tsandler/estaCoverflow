/*
 ============================================================================
 Name        : UMV.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "functions.h"

int main(){
	logs= log_create("log","UMV.c",0,LOG_LEVEL_TRACE);
	t_config *config = config_create("config.cnf");
	tamanioUMV = config_get_int_value(config, "tamanioUMV");


	log_destroy(logs);
	config_destroy(config);
	return 0;
}

/* Funcion que crea y agrega un segmento a la lista de segmentos, segun pid */
int crear_agregar_segmento(int pidInt, int tamanio){

	tablaSegUMV *unElem = malloc(sizeof(tablaSegUMV));
	int nroSeg=0;
	char* pid = (char*)string_itoa(pidInt);
	t_list *listaSeg;

	unElem->tamanioSegmento= tamanio;
	unElem->dirLogica= obtener_dir_logica(tamanio);
	unElem->dirFisica = obtener_dir_fisica(tamanio, unElem->dirLogica);
	if(dictionary_is_empty(tablaPidSeg) || !(dictionary_has_key(tablaPidSeg, pid))){
		unElem->idSegmento=1;
		listaSeg = list_create();
		list_add(listaSeg, (void*)unElem);
		dictionary_put(tablaPidSeg, pid, (void*)listaSeg);
		if(dictionary_has_key(tablaPidSeg,pid))
			log_info(logs,"El nuevo campo pid/lista se agregó correctamente");
		else
			log_error(logs,"El nuevo campo pid/lista no se agregó al diccionario");
	}else if(dictionary_has_key(tablaPidSeg, pid)){
		listaSeg = dictionary_get(tablaPidSeg,pid);
		nroSeg = obtener_nroSeg(listaSeg);
		unElem->idSegmento=nroSeg;
		list_add_in_index(listaSeg,0,(void*)unElem);
		if( (nroSeg-1) == obtener_nroSeg(listaSeg) )
			log_info(logs,"El nuevo segmento se agregó correctamente a la lista");
		else
			log_error(logs,"El nuevo segmento no se agregó a la lista correspondiente");
	}
	return unElem->dirLogica;
}
