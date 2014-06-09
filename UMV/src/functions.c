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
	listaHuecos = list_create();
	nodoHuecos* unElem= malloc(sizeof(nodoHuecos));
	unElem->dirFisica=ramUMVinicial;
	unElem->tamanioSegmento=tamanioUMV;
	list_add(listaHuecos, (void*)unElem);
}

/*Funcion que elimina estructuras del manejo de la UMV liberando el bloque de memoria principal*/
void eliminarUMV(){

	dictionary_destroy(tablaPidSeg);
	list_clean(listaHuecos);
	free(ramUMV);
	free(ramUMVinicial);
}

/* Funcion que crea y agrega un segmento a la lista de segmentos, segun pid */
int crear_agregar_segmento(int pidInt, int tamanio){
	int nroSeg=0;
	char* pid = (char*)string_itoa(pidInt);
	t_list *listaSeg;
	tablaSegUMV *unElem = malloc(sizeof(tablaSegUMV));

	unElem->tamanioSegmento= tamanio;
	unElem->dirLogica= obtener_proxima_dir_logica(tamanio,pid);
	unElem->dirFisica = obtener_proxima_dirFisica(tamanio);
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
		nroSeg = obtener_nuevo_nroSeg(listaSeg);
		unElem->idSegmento=nroSeg;
		list_add_in_index(listaSeg,0,(void*)unElem);
		if(nroSeg == (obtener_nuevo_nroSeg(listaSeg) - 1))
			log_info(logs,"El nuevo segmento se agregó correctamente a la lista");
		else
			log_error(logs,"El nuevo segmento no se agregó a la lista correspondiente");
	}
	return unElem->dirLogica;
}

/* Funcion que elimina un campo pid y sus segmentos asociados */
void destruir_segmentos(int pidInt){
	char* pid = string_itoa(pidInt);

	if( dictionary_has_key(tablaPidSeg, pid) ){
		t_list* listaSeg = dictionary_get(tablaPidSeg,pid);
		list_iterate(listaSeg,(void*)criterio_iterate);
		dictionary_remove(tablaPidSeg,pid);
		if( !dictionary_has_key(tablaPidSeg, pid)){
			log_info(logs,"Los segmentos se eliminaron correctamente");
			free(pid);
		}else
			log_error(logs,"Los sementos no se eliminaron");
	}else
		log_error(logs,"Se intento eliminar segmentos no existentes");
}

/* Funcion que elimina un segmento y agrega un nodo a la lista de huecos */
void criterio_iterate(tablaSegUMV *unElem){
	nodoHuecos* nodoNuevo = malloc(sizeof(nodoHuecos));
	nodoNuevo->dirFisica = unElem->dirFisica;
	nodoNuevo->tamanioSegmento = unElem->tamanioSegmento;
	free(unElem);
	list_add(listaHuecos, (void*)nodoNuevo);
}

/* Funcion que retorna el numero de segmento para asignarselo a un nuevo segmento */
int obtener_nuevo_nroSeg(t_list *listaSeg){
	tablaSegUMV *unElem;
	list_sort(listaSeg,(void*)sort_nroSeg);
	unElem = list_get(listaSeg, 0);
	return unElem->idSegmento +1;
}


int obtener_proxima_dir_logica(int tamanio, char* pid){
	t_list* listaSeg = dictionary_get(tablaPidSeg,pid);
	if(!list_is_empty(listaSeg)){
		list_sort(listaSeg,(void*)sort_mayor_dirLogica);
		tablaSegUMV* unElem = list_get(listaSeg,0);
		return unElem->dirLogica + unElem->tamanioSegmento + 1;
	}else
		return 0;
}

char *obtener_proxima_dirFisica(int tamanio){
	nodoHuecos* unElem;
	switch(algoritmo){
		case FIRST_FIT:
			log_info(logs,"Algoritmo de asignación: First-Fit");
			unElem = first_fit(tamanio);
			break;
		case WORST_FIT:
			log_info(logs,"Algoritmo de asignación: Worst-Fit");
			unElem = worst_fit(tamanio);
			break;
		default:
			perror("ALGORITMO NO VALIDO");
	}
	return unElem->dirFisica;
}


/*Funcion que lee un segmento de la umv y retorna un puntero a la posicion solicitada */
char *leer_segmento(int dirLog, int tamanioALeer, int offset){ //solicitar_memoria_desde_una_pos();

	bool buscar_dirLogica(tablaSegUMV* unElem){
		return dirLog == unElem->dirLogica;
	}

	t_list* listaSeg= dictionary_get(tablaPidSeg,string_itoa(pidActive));
	tablaSegUMV* unElem= list_find(listaSeg,(void*)buscar_dirLogica);
	if(unElem){
		if( unElem->dirFisica + offset + tamanioALeer <= unElem->dirFisica + unElem->tamanioSegmento ){
			char* destino = malloc(sizeof(char) * tamanioALeer);
			char* desde = unElem->dirFisica + offset;
			memcpy((void*)destino,(void*)desde,tamanioALeer);
			return destino;
		}else
			log_error(logs,"Segmentation fault");
		perror("SEGMENTATION FAULT");
	}else
		log_error(logs,"se intento acceder a una base inexistente");
	return NULL;
}

void escribir_segmento(int dirLog, int tamanioAEscribir, int offset, void* buffer){

	bool buscar_dirLogica(tablaSegUMV* unElem){
		return dirLog == unElem->dirLogica;
	}

	t_list* listaSeg= dictionary_get(tablaPidSeg,string_itoa(pidActive));
	tablaSegUMV* unElem= list_find(listaSeg,(void*)buscar_dirLogica);

	if(unElem){
		log_info(logs,"La direccion logica existe");
		if(unElem->dirFisica + offset + tamanioAEscribir <= unElem->dirFisica + unElem->tamanioSegmento){
			char* espacioSegmento = unElem->dirFisica + offset;
			memcpy((void*)espacioSegmento,buffer,tamanioAEscribir);
			printf("El segmento fue escrito");
			log_info(logs,"El segmento fue escrito");
		}else{
			log_error(logs,"SEGMENTATION FAULT");
			perror("SEGMENTATION FAULT");
		}
	}else{
		log_error(logs,"La direccion logica no existe");
		perror("DIRECCION LOGICA ERRONEA");
	}
}

void retardo(){
	usleep(retardoActual);
}

void cambiar_retardo(int retardoNuevo){
	retardoActual = retardoNuevo;
}

/*Funcion que cambia el algoritmo de asignacion de memoria*/
void cambiarAlgoritmo(int cambioAlgoritmo){
	switch(cambioAlgoritmo){
		case WORST_FIT:
			algoritmo = WORST_FIT;
			break;
		case FIRST_FIT:
			algoritmo = FIRST_FIT;
			break;
		default:
			log_error(logs,"ALGORITMO NO VALIDO");
			break;
	}
}

/*Funcion que cambia el proceso activo*/
void cambio_proceso_activo(int pid){
	pidActive = pid;
}
/*Funcion de la consola*/
char* ejec_operacion(int nroOp){
	int dirLogica=0 ,offset =0;
	void* buffer;
	int pid, tamanio, tamanioAEscribir, tamanioALeer;

	switch(nroOp){
		case LEER_SEGMENTO:
			retardo(retardoActual);
			printf("");
			return leer_segmento(dirLogica,tamanioALeer,offset);
			break;
		case ESCRIBIR_SEGMENTO:
			retardo(retardoActual);
			escribir_segmento(dirLogica,tamanioAEscribir,offset,buffer);
			return NULL;
			break;
		case CREAR_SEGMENTO:
			retardo(retardoActual);
			printf("Ingresar pid: ");
			scanf("%d",&pid);
			printf("Ingresar tamanio: ");
			scanf("%d",&tamanio);
			return string_itoa(crear_agregar_segmento(pid,tamanio));
			break;
		case ELIMINAR_SEGMENTOS:
			retardo(retardoActual);
			destruir_segmentos(pid);
			break;
		default:
			log_error(logs,"OPERACION NO VALIDA");
			break;
	free(buffer);
	}
	return NULL;
}

void dump(){
	int nroOp;
	printf("Segun indice...");
	printf("ESTRUCTURAS_MEMORIA = 0\n");
	printf("MEMORIA_PRINCIPAL = 1\n");
	printf("CONTENIDO_MEM_PPAL = 2\n");
	printf("Seleccione opcion: ");
	scanf("%d",&nroOp);
	switch(nroOp){
		case ESTRUCTURAS_MEMORIA:
			imprime_estructuras_memoria();
			/*if(opArchivo == GENERAR_ARCHIVO)
				generar_archivo();*/
			break;
		case MEMORIA_PRINCIPAL:
			imprime_estado_mem_ppal();
			/*if(opArchivo == GENERAR_ARCHIVO)
				generar_archivo();*/
			break;
		case CONTENIDO_MEM_PPAL:
			//TODO: dump: contenido memoria ppal y generar archivo
			/*if(opArchivo == GENERAR_ARCHIVO)
				generar_archivo();*/
			break;
		default:
			log_error(logs,"COMANDO NO VALIDO");
			break;
	}
}


/* Funcion que representa al funcionamiento del algoritmo first-fit de asignacion de memoria*/
nodoHuecos *first_fit(int tamanio){
	bool filtra_por_tamanio(nodoHuecos* unElem){
		return unElem->tamanioSegmento >= tamanio;
	}
	return list_remove_by_condition(listaHuecos,(void*)filtra_por_tamanio);
}

/* Funcion que representa al funcionamiento del algoritmo worst-fit de asignacion de memoria*/
nodoHuecos *worst_fit(int tamanio){
	list_sort(listaHuecos, (void*)sort_mayor_hueco);
	nodoHuecos* unElemAux = list_get(listaHuecos,0);
	list_remove(listaHuecos,0);

	bool remover_elemento(nodoHuecos* unElem){
		return unElem->dirFisica == unElemAux->dirFisica;
	}
	list_remove_by_condition(listaHuecos,(void*)remover_elemento);
	return unElemAux;
}


/* Funcion que retorna el mayor idSegmento entre dos elementos */
bool sort_nroSeg(tablaSegUMV* unElem, tablaSegUMV* otroElem){
	return unElem->idSegmento > otroElem->idSegmento;
}

/* Funcion que retorna el mayor tamanio entre dos segmentos */
bool sort_mayor_hueco(nodoHuecos* unElem, nodoHuecos* otroElem){
	return unElem->tamanioSegmento > otroElem->tamanioSegmento;
}

/* Funcion que retorna la mayor dirLogica entre dos elementos */
bool sort_mayor_dirLogica(tablaSegUMV* unElem, tablaSegUMV* otroElem){
	return unElem->dirLogica > otroElem->dirLogica;
}


void imprime_listahuecos(){
	list_iterate(listaHuecos, (void*)imprime_campos_listahuecos);
}

void imprime_campos_listatablaSegUMV(tablaSegUMV *unElem){
	printf("Nro segmento: %d\n",unElem->idSegmento);
	printf("  Dirección logica: %d\n",unElem->dirLogica);
	printf("  Dirección fisica: %s\n",unElem->dirFisica);
	printf("  Tamaño del segmento: %d\n",unElem->tamanioSegmento);
}

void imprime_campos_listahuecos(nodoHuecos *unElem){
	printf("Contenido:\n");
	printf("Dirección fisica: %s\n",unElem->dirFisica);
	printf("  Tamaño del segmento: %d\n",unElem->tamanioSegmento);

}

void imprime_estructuras_memoria(){
	void itera_diccionario(char* pid,t_list* listaSeg){
		list_iterate(listaSeg, (void*)imprime_campos_listatablaSegUMV);
	}
	printf("Reporte de: Tablas de segmentos de todos los procesos\n");
	dictionary_iterator(tablaPidSeg,(void*)itera_diccionario);
}

void imprime_estado_mem_ppal(){
	imprime_estructuras_memoria();
	printf("Bloques libres de memoria:\n");
	imprime_listahuecos();
}


