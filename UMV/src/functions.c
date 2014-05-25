#include "functions.h"

/* Funcion que solicita el bloque de memoria inicial para la UMV y
crea estructuras para el manejo de la UMV. */
void inicializar_umv(int tamanioUMV){

	ramUMV = malloc(tamanioUMV);
	ramUMVinicial = ramUMV;
	if(!ramUMV)
		log_error(logs,"La UMV no se inicializo correctamente");
	else
		log_info(logs,"La UMV se inicializo correctamente");

	tablaPidSeg = dictionary_create();
	listaDirLogica = list_create();
	listaHuecos = list_create();
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

/* Funcion que retorna el numero de segmento para asignarselo a un nuevo segmento */
int obtener_nroSeg(t_list *listaSeg){

	tablaSegUMV *nodo;
	int nroSeg;
	list_sort(listaSeg, (void*) sort_nroSeg);
	nodo = list_get(listaSeg, 0);
	nroSeg = nodo->idSegmento;
	return nroSeg+1;
}
/* Funcion que retorna el mayor idSegmento entre dos elementos */
bool sort_nroSeg(tablaSegUMV* unElem, tablaSegUMV* otroElem){
	return unElem->idSegmento > otroElem->idSegmento;
}

/* Funcion que obtiene la direccion logica */
int obtener_dir_logica(int tamanio){
	nodoDirLog *unElem=malloc(sizeof(nodoDirLog));

	if(list_is_empty(listaDirLogica))
		baseLogica=1;
	else{
		unElem->dirLog=baseLogica;
		unElem->tamanio=tamanio;
		unElem->dirFisica= obtener_dir_fisica(tamanio,unElem->dirLog);
		list_add(listaDirLogica, (void*)unElem);
		baseLogica = baseLogica + tamanio + 1;
	}
	return unElem->dirLog;
}

/* Funcion que obtiene la direccion fisica a partir de una direccion logica */
char *obtener_dir_fisica(int tamanio,int dirLogica){
	char *aux= ramUMV;
	nodoDirLog *unElemAux;
	bool criterio(nodoDirLog *unElem){
		return unElem->dirLog == dirLogica;
	}
	unElemAux = list_find( listaDirLogica,(void*)criterio);
	if(unElemAux){
		ramUMV = ramUMV + tamanio;
		return aux;
	}else
		return unElemAux->dirFisica;
}


/* Funcion que elimina un campo pid y sus segmentos asociados */
void destruir_segmentos(int pidInt){
	char* pid = string_itoa(pidInt);
	t_list* listaSeg;

	if( dictionary_has_key(tablaPidSeg, pid) ){
		listaSeg = dictionary_get(tablaPidSeg,pid);
		list_iterate(listaSeg, (void*)criterio_iterate );
		dictionary_remove(tablaPidSeg,pid);
		if( !dictionary_has_key(tablaPidSeg, pid))
			log_info(logs,"Los segmentos se eliminaron correctamente");
		else
			log_error(logs,"Los sementos no se eliminaron");
	}else
		log_error(logs,"Se intento eliminar segmentos no existentes");
}

/* Funcion que elimina un segmento y agrega un nodo a la lista de huecos */
void criterio_iterate(tablaSegUMV *unElem){
	nodoHuecos* nodoNuevo = malloc(sizeof(nodoHuecos));

	/*Funcion que retorna 1 si encuentra un elemento de la lista especifico */
	bool criterio_remover(nodoDirLog *elemLista){
		return unElem->dirLogica == elemLista->dirLog;
	}
	list_remove_by_condition(listaDirLogica, (void*)criterio_remover);

	nodoNuevo->dirFisica = unElem->dirFisica;
	nodoNuevo->tamanioSegmento = unElem->tamanioSegmento;
	list_add(listaHuecos, (void*)nodoNuevo);
}


/*Funcion que elimina estructuras del manejo de la UMV liberando el bloque de memoria principal*/
void eliminarUMV(){

	dictionary_destroy(tablaPidSeg);
	list_clean(listaDirLogica);
	list_clean(listaHuecos);
}
