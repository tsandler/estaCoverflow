#include "functions.h"

/*Funcion que llama el hilo cada vez que se conecta algun CPU a la UMV*/
void funcion_CPU(estructura_hilo* hilo){
	int pid;
	t_length* tam = malloc(sizeof(t_length));
	datos_acceso* etiq;
	unsigned char* codigo;

	while(1){
		if(!recibirMenu(hilo->socket, tam, logs)){
			log_error(logs, "Se produjo un error recibiendo el menu");
			break;
		}

		switch(tam->menu){
			case PID_ACTUAL:
				if(!recibirDato(hilo->socket, tam->length, (void*)&pid, logs)){
					log_error(logs, "Se produjo un error recibiendo el pid");
					break;
				}
				cambiar_pid_activo(pid);
				break;
			case ESCRIBIR_SEGMENTO:
				if(!recibirDato(hilo->socket,tam->length,(void*)&codigo,logs)){
					log_error(logs,"Se produjo un error recibiendo el codigo");
					break;
				}
				if(!recibirDatos(hilo->socket,tam,(void*)etiq,logs)){
					log_error(logs,"Se produjo un error recibiendo la esctructura");
					break;
				}
				escribir_segmento(etiq->base,etiq->tamanio,etiq->offset,(void*)codigo);
				break;
			case PEDIR_SENTENCIA:
				if(!recibirDato(hilo->socket,tam->length,(void*)etiq,logs)){
					log_error(logs,"Se produjo un error recibiendo la esctructura");
					break;
				}
				codigo = leer_segmento(etiq->base,etiq->tamanio,etiq->offset);
				//tam->menu = TE_MANDO_EL_SEGMENTO;
				tam->length = etiq->tamanio;
				if(!enviarDatos(hilo->socket, tam, codigo, logs))
					log_error(logs,"Error al enviarse el codigo");
				break;
			case LEER_SEGMENTO:
				if(!recibirDato(hilo->socket,tam->length,(void*)&etiq,logs)){
					log_error(logs,"Se produjo un error recibiendo la esctructura");
					break;
				}
				codigo = leer_segmento(etiq->base,etiq->tamanio,etiq->offset);
				//tam->menu = TE_MANDO_EL_SEGMENTO;
				tam->length = etiq->tamanio;
				enviarDatos(hilo->socket, tam, codigo, logs);
				break;
			default:
				log_error(logs,"operacion invalida para CPU");
				break;
		}
	}
}

/*Funcion que llama el hilo cuando se conecta el KERNEL a la UMV*/
void funcion_kernel(estructura_hilo* hilo){
	int pid;
	t_length* tam = malloc(sizeof(t_length));
	datos_acceso* etiq;
	unsigned char* codigo;
	while(1){
		if(!recibirMenu(hilo->socket, tam, logs)){
			log_error(logs, "Se produjo un error recibiendo el menu");
			break;
		}

		datos_crearSeg* pidTam = malloc(sizeof(datos_crearSeg));
		printf("Menu: %d",tam->menu);
		switch(tam->menu){
			case PID_ACTUAL:
				if(!recibirDato(hilo->socket, tam->length, (void*)&pid, logs)){
					log_error(logs, "Se produjo un error recibiendo el pid");
					break;
				}
				cambiar_pid_activo(pid);
				break;
			case ESCRIBIR_SEGMENTO:
				if(!recibirDato(hilo->socket,tam->length,(void*)&codigo,logs)){
					log_error(logs,"Se produjo un error recibiendo el codigo");
					break;
				}
				if(!recibirDatos(hilo->socket,tam,(void*)&etiq,logs)){
					log_error(logs,"Se produjo un error recibiendo la esctructura");
					break;
				}
				escribir_segmento(etiq->base,etiq->tamanio,etiq->offset,codigo);
				break;
			case CREAR_SEGMENTO:
				if(!recibirDato(hilo->socket, tam->length, (void*)&pidTam, logs)){
					log_error(logs, "Se produjo un error recibiendo pid-tamanio");
					break;
				}
				int baseLog = crear_agregar_segmento(pidTam->pid,pidTam->tamanio);
				tam->length = sizeof(int);
	//			tam->menu = TE MANDO LA BASE DEL SEGMENTO
				enviarDatos(hilo->socket, tam, (void*)&baseLog, logs);
				break;
			case ELIMINAR_SEGMENTOS:
				if(!recibirDato(hilo->socket, tam->length, (void*)&pid, logs)){
					log_error(logs, "Se produjo un error recibiendo el pid");
					break;
				}
				destruir_segmentos(pid);
				break;
			case SOY_KERNEL:
				log_debug(logs,"alal");
				break;
			default:
				log_error(logs,"operacion invalida para Kernel");
				retardo();
				break;
		}
	}
}


/* Funcion que solicita el bloque de memoria inicial para la UMV y
crea estructuras para el manejo de la UMV. */
void inicializar_umv(int tamanioUMV){

	tablaPidSeg = dictionary_create();
	listaHuecos = list_create();
	nodoHuecos* unElem= malloc(sizeof(nodoHuecos));
	unElem->dirFisica=malloc(tamanioUMV);
	unElem->tamanioSegmento=tamanioUMV;
	ramUMVInicial = unElem->dirFisica;
	list_add(listaHuecos,unElem);
}

/*Funcion que elimina estructuras del manejo de la UMV liberando el bloque de memoria principal*/
void eliminarUMV(){
	log_debug(logs,"Comienza a eliminar umv");
	dictionary_clean_and_destroy_elements(tablaPidSeg, (void*)vaciarLista);
	log_debug(logs,"Ya elimino el diccionario");
	list_clean_and_destroy_elements(listaHuecos, free);
	free(listaHuecos);
	free(tablaPidSeg);
	log_debug(logs,"La umv fue eliminada");
}

/*Funcion que valida que el archivo de configuracion tenga todos los campos*/
bool archivo_config_valido(){
	if(!config_has_property(config, "TAMANIO_UMV"))
		return 0;

	if(!config_has_property(config, "RETARDO"))
		return 0;

	if(!config_has_property(config, "ALGORITMO"))
		return 0;

	if(!config_has_property(config, "PUERTO"))
		return 0;
	return 1;
}

/*Funcion que inicializa las variables globales respecto del config*/
void inicializar_var_config(){
	puerto = config_get_int_value(config, "PUERTO");
	tamanioUMV = config_get_int_value(config, "TAMANIO_UMV");
	retardoActual = config_get_int_value(config,"RETARDO");
	algoritmoActual = config_get_int_value(config,"ALGORITMO");
}


void vaciarLista(t_list* listaSeg){
	list_clean_and_destroy_elements(listaSeg,free);
}

/* Funcion que crea y agrega un segmento a la lista de segmentos, segun pid.
   Retorna la direccion logica del segmento */

int crear_agregar_segmento(int pidInt, int tamanio){
	int nroSeg=0;
	char* pid = string_itoa(pidInt);
	t_list *listaSeg;
	tablaSegUMV *unElem = malloc(sizeof(tablaSegUMV));

	unElem->tamanioSegmento= tamanio;
	unElem->dirLogica= obtener_proxima_dir_logica(tamanio,pid);
	log_debug(logs,"Se obtuvo la dirLogica");
	unElem->dirFisica = obtener_proxima_dirFisica(tamanio);
	log_debug(logs,"Se obtuvo la dirFisica");
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
		list_iterate(listaSeg,(void*)elimina_segmento_agrega_hueco);
		dictionary_remove(tablaPidSeg,pid);

		if( dictionary_has_key(tablaPidSeg, pid)){
			log_error(logs,"Los sementos no se eliminaron");
		}else
			log_info(logs,"Los segmentos se eliminaron correctamente");
	}else{
		log_error(logs,"Se intento eliminar segmentos no existentes");
		free(pid);
	}
}

/* Funcion que elimina un segmento y agrega un nodo a la lista de huecos */
void elimina_segmento_agrega_hueco(tablaSegUMV *unElem){
	nodoHuecos* nodoNuevo=malloc(sizeof(nodoHuecos));
	nodoNuevo->dirFisica = unElem->dirFisica;
	nodoNuevo->tamanioSegmento = unElem->tamanioSegmento;
	free(unElem);
	list_add(listaHuecos,nodoNuevo);
}

/* Funcion que retorna el numero de segmento para asignarselo a un nuevo segmento */
int obtener_nuevo_nroSeg(t_list *listaSeg){
	tablaSegUMV *unElem;
	list_sort(listaSeg,(void*)sort_nroSeg);
	unElem = list_get(listaSeg, 0);
	return unElem->idSegmento +1;
}

/* Funcion que retorna una dirLogica para asignarsela a un nuevo segmento */

int obtener_proxima_dir_logica(int tamanio, char* pid){
	log_debug(logs,"Entra a obtener direccion logica");
	if(dictionary_has_key(tablaPidSeg,pid)){
		t_list* listaSeg = dictionary_get(tablaPidSeg,pid);
		list_sort(listaSeg,(void*)sort_mayor_dirLogica);
		tablaSegUMV* unElem = list_get(listaSeg,0);
		return unElem->dirLogica + unElem->tamanioSegmento + 1;
	}else
		return 0;
}

/*Funcion que obtiene una nueva direccion fisica (segun algoritmo)*/
char *obtener_proxima_dirFisica(int tamanio){
	log_debug(logs,"Entra a obtener dir fisica");
	switch(algoritmoActual){
		case FIRST_FIT:
			log_debug(logs,"Algoritmo de asignación: First-Fit");
			return first_fit(tamanio);
		case WORST_FIT:
			log_debug(logs,"Algoritmo de asignación: Worst-Fit");
			return worst_fit(tamanio);
	}
	return NULL;
}

// FIXME: llega base,offset y tamanio. yo devuelvo [offset,tamanio]


/*Funcion que lee un segmento de la umv y retorna un puntero a la posicion solicitada */
unsigned char *leer_segmento(int dirLog, int tamanioALeer, int offset){ //solicitar_memoria_desde_una_pos();

	bool buscar_dirLogica(tablaSegUMV* unElem){
		return dirLog == unElem->dirLogica;
	}

	t_list* listaSeg= dictionary_get(tablaPidSeg,string_itoa(pidActive));
	tablaSegUMV* unElem= list_find(listaSeg,(void*)buscar_dirLogica);
	if(unElem){
		if( unElem->dirFisica + offset + tamanioALeer <= unElem->dirFisica + unElem->tamanioSegmento ){
			unsigned char* destino = malloc(sizeof(char) * tamanioALeer);
			char* desde = unElem->dirFisica + offset;
			memcpy((void*)destino,(void*)desde,tamanioALeer);
			return destino;
		}else
			log_error(logs,"Segmentation fault");
	}else
		log_error(logs,"se intento acceder a una base inexistente");
	return NULL;
}

/*Funcion que escribe el buffer en un segmento determinado*/
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
			memcpy(espacioSegmento,buffer,tamanioAEscribir);
			printf("El segmento fue escrito");
			log_info(logs,"El segmento fue escrito");
		}else{
			log_error(logs,"SEGMENTATION FAULT");
			printf("Segmentation fault");
		}
	}else{
		log_error(logs,"La direccion logica no existe");
	}
}

void retardo(){
	usleep(retardoActual);
}

/*Funcion que cambia el retardo actual -en milisegundos-*/
void cambiar_retardo(int retardoNuevo){
	retardoActual = retardoNuevo;
}

/*Funcion que cambia el algoritmo de asignacion de memoria*/
void cambiarAlgoritmo(int cambioAlgoritmo){
	switch(cambioAlgoritmo){
		case WORST_FIT:
			algoritmoActual = WORST_FIT;
			break;
		case FIRST_FIT:
			algoritmoActual = FIRST_FIT;
			break;
		default:
			log_error(logs,"ALGORITMO NO VALIDO");
			break;
	}
}

/*Funcion que cambia el id del proceso activo*/
void cambiar_pid_activo(int pid){
	pidActive = pid;
}


/*Funcion que llama la consola cuando debe ejecutar una operacion*/
unsigned char* ejec_operacion(int nroOp){
	int dirLogica=0, offset=0;
	void* buffer = NULL; //FIXME: preguntar cómo se inicializa
	int pid, tamanio=0, tamanioAEscribir=0, tamanioALeer=0;
	int genArch;

	switch(nroOp){
		case LEER_SEGMENTO:
			retardo(retardoActual);
			//////
			printf("Ingresar pid: ");
			scanf("%d",&pid);
			cambiar_pid_activo(pid);
			//////
			printf("Ingresar tamanio a leer: ");
			scanf("%d",&tamanio);
			printf("Ingresar la direccion logica: ");
			scanf("%d",&dirLogica);
			printf("Ingresar offset: ");
			scanf("%d",&offset);
			unsigned char* resultado = leer_segmento(dirLogica,tamanioALeer,offset);
			printf("Desea generar un archivo con el resultado? 0=no 1=si: ");
			scanf("%d",&genArch);
			if(genArch)
				generar_archivo(resultado,nroOp);
			return resultado;
			break;
		case ESCRIBIR_SEGMENTO:
			retardo(retardoActual);
			//////
			printf("Ingresar pid: ");
			scanf("%d",&pid);
			cambiar_pid_activo(pid);
			//////
//			FIXME: como poner el buffer que se escribe
			escribir_segmento(dirLogica,tamanioAEscribir,offset,buffer);
			printf("Desea generar un archivo con el resultado? 0=no 1=si: ");
			scanf("%d",&genArch);
			if(genArch)
				//generar_archivo(resultado,nroOp);
			return NULL;
			break;
		case CREAR_SEGMENTO:
			retardo(retardoActual);
			printf("Ingresar pid: ");
			scanf("%d",&pid);
			printf("Ingresar tamanio: ");
			scanf("%d",&tamanio);
			return (unsigned char*)crear_agregar_segmento(pid,tamanio);
			break;
		case ELIMINAR_SEGMENTOS:
			retardo(retardoActual);
			printf("Ingresar pid: ");
			scanf("%d",&pid);
			destruir_segmentos(pid);
			break;
		default:
			log_error(logs,"OPERACION NO VALIDA");
			break;
	}
	return NULL;
}

void generar_archivo(unsigned char* resultado, int nroOp){
	FILE* arch;
	char* nombre;
	log_debug(logs,"entra a generar archivo");

	switch(nroOp){
		case LEER_SEGMENTO:
			log_debug(logs,"entra a leerSeg");
			nombre ="leer";
			arch = txt_open_for_append(nombre);
			txt_write_in_file(arch,(char*)resultado);
			break;
		case ESCRIBIR_SEGMENTO:
			log_debug(logs,"entra a escribirSeg");
			nombre = "escribir";
			arch = txt_open_for_append(nombre);
			txt_write_in_file(arch,(char*)resultado);
			break;
		default:
			break;
	}
}

void dump(){
	int nroOp, opArchivo;
	printf("Segun indice...\n");
	printf("ESTRUCTURAS_MEMORIA = 0\n");
	printf("MEMORIA_PRINCIPAL = 1\n");
	printf("CONTENIDO_MEM_PPAL = 2\n");
	printf("Seleccione opcion: ");
	scanf("%d",&nroOp);
	switch(nroOp){
		case ESTRUCTURAS_MEMORIA:
			imprime_estructuras_memoria();
			printf("Desea generar archivo <1/0>: ");
			scanf("%d",&opArchivo);
//			if(opArchivo == GENERAR_ARCHIVO)
//				generar_archivo(unsigned char* resultado, int nroOp); TODO: generarArchivo
			break;
		case MEMORIA_PRINCIPAL:
			imprime_estado_mem_ppal();
//			if(opArchivo == GENERAR_ARCHIVO)
//				generar_archivo();
			break;
		case CONTENIDO_MEM_PPAL:
			//TODO: dump: contenido memoria ppal y generar archivo
//			if(opArchivo == GENERAR_ARCHIVO)
//				generar_archivo();
			break;
		default:
			log_error(logs,"COMANDO NO VALIDO");
			break;
	}
}


/* Funcion que representa al funcionamiento del algoritmo first-fit de asignacion de memoria*/
char *first_fit(int tamanio){ //

	bool filtra_por_tamanio(nodoHuecos* unElem){
		return unElem->tamanioSegmento >= tamanio;
	}
	nodoHuecos* unElem = list_remove_by_condition(listaHuecos,(void*)filtra_por_tamanio);

	if(!unElem){
		//llamar a compactar
		unElem = list_remove_by_condition(listaHuecos,(void*)filtra_por_tamanio);
		if(!unElem){
			log_error(logs,"Memory overload");
			printf("Memory overload");
			return string_itoa(-1);
		}
	}
	return manejo_memoria(unElem,tamanio);
}

/* Funcion que representa al funcionamiento del algoritmo worst-fit de asignacion de memoria*/
char *worst_fit(int tamanio){
	list_sort(listaHuecos, (void*)sort_mayor_hueco);
	nodoHuecos* unElem = list_remove(listaHuecos,0);

	if(!unElem){
		//llamar a compactar
		list_sort(listaHuecos, (void*)sort_mayor_hueco);
		nodoHuecos* unElem = list_remove(listaHuecos,0);
		if(!unElem){
			log_error(logs,"Memory overload");
			printf("Memory overload");
			return string_itoa(-1);
		}
	}
	return manejo_memoria(unElem,tamanio);
}

char *manejo_memoria(nodoHuecos* unElem, int tamanio){
	char* nuevaDirFisica=malloc(sizeof(char)*tamanio);
	memcpy(nuevaDirFisica,unElem->dirFisica,tamanio);
	unElem->dirFisica = unElem->dirFisica + tamanio;
	unElem->tamanioSegmento = unElem->tamanioSegmento - tamanio;
	list_add(listaHuecos,(void*)unElem);
	return nuevaDirFisica;
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
	log_info(logs,"Se imprime lista huecos");
	list_iterate(listaHuecos, (void*)imprime_campos_listahuecos);
}

void imprime_campos_listatablaSegUMV(tablaSegUMV *unElem){
	printf("Nro segmento: %d\n",unElem->idSegmento);
	printf("  Dirección logica: %d\n",unElem->dirLogica);
	printf("  Tamaño del segmento: %d\n",unElem->tamanioSegmento);
}

void imprime_campos_listahuecos(nodoHuecos *unElem){
	printf("Contenido:\n");
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


void consola(){
	log_debug(logs,"Entra a consola");
	int pid, cambioAlgoritmo ,nroOp, operacion, retardoNuevo;
	int i=1;
	while(i){
		printf("\nSeleccione la operación segun...\n");
		printf("OPERACION = 1\n");
		printf("RETARDO = 2\n");
		printf("ALGORITMO = 3\n");
		printf("COMPACTACION = 4\n");
		printf("DUMP = 5\n");
		printf("Operacion solicitada: ");
		scanf("%d",&operacion);

		switch(operacion){
			case CAMBIO_PROCESO_ACTIVO:
				printf("Ingrese nuevo pid: ");
				scanf("%d",&pid);
				cambiar_pid_activo(pid);
				break;
			case OPERACION:
				printf("Ingrese operacion segun...\n");
				printf("LEER_SEGMENTO = 1\n");
				printf("ESCRIBIR_SEGMENTO = 2\n");
				printf("CREAR_SEGMENTO = 3\n");
				printf("ELIMINAR_SEGMENTOS = 4\n");
				printf("Ingrese operacion: ");
				scanf("%d",&nroOp);
				ejec_operacion(nroOp);

				break;
			case CAMBIAR_RETARDO:
				printf("Ingrese nuevo retardo: ");
				scanf("%d",&retardoNuevo);
				cambiar_retardo(retardoNuevo);
				break;
			case CAMBIAR_ALGORITMO:
				printf("Ingrese opcion segun...\n");
				printf("FIRST_FIT = 0\nWORST_FIT = 1\n");
				printf("Opcion: ");
				scanf("%d",&cambioAlgoritmo);
				cambiarAlgoritmo(cambioAlgoritmo);
				break;
			case DUMP:
				dump();
				break;
			default:
				printf("Operacion invalida\n");
				break;
		}
		printf("Realizar otra operacion? s=1:");
		scanf("%d",&i);

	}
}
//
//void compactar_memoria(){
//	ramAux = ramUMVInicial;
//	tamHueco = 0;
//
//	while(ramAux < ramUMVInicial + tamanioUMV){
//		nodoHuecos* unElem = list_remove_by_condition(listaHuecos,(void*)buscar_ramSiguiente);
//		if(unElem){
//			buscar_hueco_para_compactar(unElem);
//		}else{
//			buscar_segmento_y_desplazarlo();
//			tamHueco = 0;
//		}
//	}
//}
//
//void buscar_hueco_para_compactar(nodoHuecos* unElem){
//	char* dirHueco = unElem->dirFisica;
//	do{
//		tamHueco += unElem->tamanioSegmento;
//		ramAux = unElem->dirFisica + unElem->tamanioSegmento;
//		unElem = list_remove_by_condition(listaHuecos,(void*)buscar_ramSiguiente);
//	}while(unElem);
//}
//
//void buscar_segmento_y_desplazarlo(){
//	dictionary_iterator(tablaPidSeg,busca_seg_en_diccionario);
//}
//
//void busca_seg_en_diccionario(char* pid,t_list* listaSeg){
//
//	tablaSegUMV* unElem = list_find(listaSeg,buscar_ramAux);
//
//	if(tamHueco >= unElem->tamanioSegmento){
//		nodoHuecos* nuevoElem;
//		nuevoElem->dirFisica = unElem->dirFisica;
//		nuevoElem->tamanioSegmento = unElem->tamanioSegmento;
//		list_add(listaHuecos,nuevoElem);
//	}else{
//
//	}
//	ramAux = unElem->dirFisica + unElem->tamanioSegmento;
//	unElem->dirFisica = memmove(dirHueco,unElem->dirFisica,unElem->tamanioSegmento);
//}
//
//bool buscar_ramAux(tablaSegUMV* unElem){
//	return unElem->dirFisica == ramAux;
//}
//
//bool buscar_ramSiguiente(nodoHuecos* unElem){
//	return unElem->dirFisica == ramAux;
//}
