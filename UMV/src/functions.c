#include "functions.h"
#include <libs/sockets.h>
#include <pthread.h>
#include <libs/estructurasParser.h>

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

	int nroSeg=0;
	char* pid = (char*)string_itoa(pidInt);
	t_list *listaSeg;
	tablaSegUMV *unElem = malloc(sizeof(tablaSegUMV));
	se_inicializo_puntero((void*)unElem);

	unElem->tamanioSegmento= tamanio;
	unElem->dirLogica= obtener_dir_logica(tamanio,pid);
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
int obtener_dir_logica(int tamanio, char* pid){
	nodoDirLog *unElem=malloc(sizeof(nodoDirLog));

	if(list_is_empty(listaDirLogica))
		baseLogica=1;
	else{
		unElem->dirLog=baseLogica;
		unElem->tamanio=tamanio;
		unElem->dirFisica= obtener_dir_fisica(tamanio,unElem->dirLog);
		unElem->pid= pid;
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

void se_inicializo_puntero(void* unElem){
	if(unElem)
		log_info(logs,"Se otorgo la porcion de memoria");
	else
		log_error(logs,"No se otorgo la porcion de memoria");
}

/*Funcion que elimina estructuras del manejo de la UMV liberando el bloque de memoria principal*/
void eliminarUMV(){

	dictionary_destroy(tablaPidSeg);
	list_clean(listaDirLogica);
	list_clean(listaHuecos);
}

/*Funcion que lee un segmento de la umv y retorna un puntero a la posicion solicitada */
char *leer(int dirLog, int tamanioALeer, int offset){ //solicitar_memoria_desde_una_pos();
	nodoDirLog *nodoAux= malloc(sizeof(nodoDirLog));
	se_inicializo_puntero((void*)nodoAux);
	char* destino = malloc(sizeof(char));
	se_inicializo_puntero((void*)destino);
	char* desde = malloc(sizeof(char));
	se_inicializo_puntero((void*)desde);

	bool existe_dir_logica(nodoDirLog *elemLista){
		return  dirLog == elemLista->dirLog;
	}
	nodoAux =list_find(listaDirLogica, (void*)existe_dir_logica);

	if(nodoAux){
		if( nodoAux->dirFisica + offset + tamanioALeer <= nodoAux->dirFisica + nodoAux->tamanio ){
			desde = nodoAux->dirFisica + offset;
			memcpy((void*)destino,(void*)desde,tamanioALeer);
			free(nodoAux);
			free(desde);
			return destino;
		}else
			log_error(logs,"Segmentation fault");
	}else
		log_error(logs,"se intento acceder a una base inexistente");
	free(nodoAux);
	free(desde);
	free(destino);
	return 0;
}

//Distingue que proceso se conecto CPU o KERNEL y por cada conexion que e
bool handshake(int procesoConectado){
	int processName;
	char*datos;
	t_length*tam = malloc(sizeof(tam));
	int esCPU;
	int esKernel;
	processName = recibirDatos(procesoConectado,tam, (void*)&datos,logs);
	if(processName<0){
		log_error(logs,"no se puedieron recibir los datos correctamente");
		return 0;
	}
	//a modo de test:
	char* str = string_from_format("%s \n", datos);
	printf("%s",str);

	if (strcmp(str,"soy CPU"))
		return 1;
	else if(strcmp(str,"soy kernel"))
		return 2;
	else
		return 0;
}

void escucharKernel(int*pConectado){
	int datosKernel;
	t_length*tam=malloc(sizeof(tam));
	pthread_t thread1;
	pthread_t thread2;
	int operacionKernel;
	 datosKernel = recibirDatos(pConectado,tam,(void*)&operacionKernel,logs);
	 switch(*operacionKernel);
	 case RESERVAR:
	 pthread_create(&thread1,NULL,reservar, (void*)pConectado);
	 case ESCRIBIR:
	 pthread_create(&thread2,NULL,escribir,(void*)pConectado);
	 return;
}

void reservar(int pidInt,int tamanio);
/*LO QUE TENGA QUE HACER LA UMV CON RESERVAR*/
void escribir(int base, int offset, int tamanio);
/*LO QUE TENGA Q HACER LA UMV CON ESCRIBIR*/



void escucharCPU(int*pConectado){
	int datosCPU;
	t_length*tam=malloc(sizeof(tam));
	int operacionCPU;
	pthread_t thread1;
	pthread_t thread2;
	datosCPU = recibirDatos(pConectado,tam,(void*)&operacionCPU,logs);
	switch(*operacionCPU);
	case PEDIR_SEGMENTO:
	pthread_create(&thread1,NULL,pedir_segmento,(void*)pConectado);
	case PEDIR_SENTENCIA:
	pthread_create(&thread2,NULL,pedir_sentencia,(void*)pConectado);
	return;
}


void pedir_segmento(int leer);
/*LO QUE TENGA QUE HACER LA UMB CON PEDIR_SEGMENTO*/

void pedir_sentencia(int base, int tamanio);

