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
		return unElem->dirLog == dirLogica ? 1 : 0;
	}
	unElemAux = list_find( listaDirLogica,(void*)criterio);
	if(unElemAux){
		ramUMV = ramUMV + tamanio;
		return aux;
	}else
		return unElemAux->dirFisica;
}

/*Funcion que elimina estructuras del manejo de la UMV liberando
 el bloque de memoria principal*/
void eliminarUMV(){

	dictionary_destroy(tablaPidSeg);
	list_clean(listaDirLogica);
	list_clean(listaHuecos);
}
