#include "functions.h"

extern sem_t yaEscribio;
extern sem_t mutexOpera;
extern int retardoActual;
extern int encontroHueco;
extern int algoritmoActual;

t_elem_cola_cpu *desencolar_peticion(int socket){
	t_elem_cola_cpu* unaPeticion;
	int i= colaCPUs->elements_count;
	while(i>0){
		if(list_size(colaCPUs) == 1){
			unaPeticion = list_get(colaCPUs,0);
			if(unaPeticion->socket ==  socket)
				return list_remove(colaCPUs,0);
		}else{
			unaPeticion = list_get(colaCPUs,i);
			if(unaPeticion->socket ==  socket)
				return list_remove(colaCPUs,i);
		}
		i--;
	}
	return NULL;
}

bool esCorrecto_pid(int pid){
	if(pid >= 0 && pid < 100)
		return 1;
	return 0;
}

/*Funcion que saca las peticiones de un cpu cuando se cae*/
void bajar_cpu(int socket){

	bool eliminar_peticiones(t_elem_cola_cpu* unaPeticion){
		return unaPeticion->socket == socket;
	}

	void libera_datos(t_elem_cola_cpu* unaPeticion){
	//	free(unaPeticion->tama);
	}

	list_remove_and_destroy_by_condition(colaCPUs,(void*)eliminar_peticiones,(void*)libera_datos);
}

/*Funcion que llama el hilo cada vez que se conecta algun CPU a la UMV*/
void funcion_CPU(int socket){

	int* pid, pidLocal=-1;
	int termina=0;
	t_length* tam = malloc(sizeof(t_length));
	t_etiqueta* etiq = malloc(sizeof(t_etiqueta));
	t_elem_cola_cpu* unaPeticion = malloc(sizeof(t_elem_cola_cpu));
	unaPeticion->tama = malloc(sizeof(t_length));
	unsigned char* codigo;
	int base, offset, tamanio, nroSegmento;
	char* buffer;
	int seBajoCPU=0;

	tam->menu = OK;

	if(!recibirMenu(socket, tam, logs)){
		log_error(logs, "Se produjo un error recibiendo una peticion");
	}

	if(tam->menu == OK){
		log_error(logs,"[CPU] Se perdio la conexion. Baja al cpu");
		int ret=1;
		pthread_exit(&ret);
	}
	int esPrimerEjecucion = 1;

	sem_wait(&yaEscribio);
	sem_wait(&yaEscribio);
	if(segEscritos == 3)
		sem_wait(&yaEscribio);

	while(1){
		seBajoCPU=0;

		if(tam->menu != OK && esPrimerEjecucion){
			unaPeticion->socket = socket;
			unaPeticion->tama = tam;
			list_add_in_index(colaCPUs,0,unaPeticion);
			unaPeticion = desencolar_peticion(socket);
			esPrimerEjecucion = 0;

			if(!unaPeticion)
				log_error(logs,"[HILO CPU] No hay peticion");

		}else if(!esPrimerEjecucion){

			tam->menu = OK;

			if(!recibirMenu(socket, tam, logs)){
				log_error(logs, "Se produjo un error recibiendo una peticion");
				break;
			}

			unaPeticion->socket = socket;
			unaPeticion->tama = tam;
			list_add_in_index(colaCPUs,0,unaPeticion);
			unaPeticion = desencolar_peticion(socket);

			if(unaPeticion->tama->menu==OK){
				log_error(logs,"[CPU] Se perdio la conexion. Baja al cpu");
				bajar_cpu(socket);
				seBajoCPU=1;

				if(!list_size(colaCPUs))
					log_info(logs,"[CPU] No hay cpus conectados. Se baja la conexion");

				int ret=1;
				pthread_exit(&ret);

				if(!recibirMenu(socket, tam, logs)){
					log_error(logs, "Se produjo un error recibiendo una peticion");
					break;
				}

				unaPeticion->socket = socket;
				unaPeticion->tama = tam;
				list_add_in_index(colaCPUs,0,unaPeticion);
				unaPeticion = desencolar_peticion(socket);
			}

			if(!unaPeticion)
				log_error(logs,"[HILO CPU] No hay peticion");

		}

		switch(unaPeticion->tama->menu){
			case PID_ACTUAL:
				sem_wait(&mutexOpera);
				retardo();
				if(!recibirDato(socket, tam->length, (void*)&pid, logs)){
					log_error(logs, "Se produjo un error recibiendo el pid");
					break;
				}
				if( !esCorrecto_pid(*pid) ){
					log_error(logs,"[HILO CPU] Se recibio un pid invalido");
					termina = 1;
					break;
				}
				pidLocal = *pid;
				log_info(logs,"[HILO CPU] Se cambio el pid activo a: %d",pidLocal);
				sem_post(&mutexOpera);
				break;
			case ESCRIBIR_SEGMENTO:
				retardo();
				sem_wait(&mutexOpera);
				if(!recibirDato(socket,tam->length,(void*)&etiq,logs)){
					log_error(logs,"Se produjo un error recibiendo la esctructura");
					break;
				}

				if( validacion_base(etiq->base) ){
					log_error(logs,"[HILO CPU] Error en la base al intentar escribir el segmento");
					termina = 1;
					break;
				}
				base=etiq->base;
				offset=etiq->offset;
				tamanio=etiq->tamanio;

				if(!recibirDatos(socket,tam,(void*)&buffer,logs)){
					log_error(logs,"[HILO CPU] Se produjo un error recibiendo el codigo");
					break;
				}

				int haySegFault = escribir_segmento(base,tamanio,offset,buffer,pidLocal);

				if(haySegFault == -1){ //FIXME
					tam->menu= SEG_FAULT;
					enviarMenu(socket,tam,logs);
					log_debug(logs,"[HILO CPU] El CPU: %d Escribio el segmento nro: %d",socket,nroSegmento);
				}else{
					tam->menu= OK;
					enviarMenu(socket,tam,logs);
				}
				nroSegmento++;

				sem_post(&mutexOpera);
				break;

			case PEDIR_SENTENCIA:
				retardo();
				sem_wait(&mutexOpera);

				if(!recibirDato(socket,tam->length,(void*)&etiq,logs)){
					log_error(logs,"Se produjo un error recibiendo la esctructura");
					break;
				}

				codigo = leer_segmento(etiq->base,etiq->tamanio,etiq->offset,pidLocal);

				if(!codigo){
					termina = 1;
					log_error(logs,"[HILO CPU] No se envió la sentencia. Error en la etiqueta. Finaliza el CPU");
					break;
				}
				tam->length = etiq->tamanio;
				if(!enviarDatos(socket, tam, codigo, logs))
					log_error(logs,"Error al enviarse el codigo");

				if(!recibirDatos(socket,tam,(void*)&etiq,logs)){
					log_error(logs,"Se produjo un error recibiendo la esctructura");
					break;
				}

				codigo = leer_segmento(etiq->base,etiq->tamanio,etiq->offset,pidLocal);

				if(!codigo){
					termina = 1;
					log_error(logs,"[HILO CPU] No se envió la sentencia. Error en la etiqueta. Finaliza el CPU");
					break;
				}

				tam->length = etiq->tamanio;

				if(!enviarDatos(socket, tam, codigo, logs))
					log_error(logs,"Error al enviarse la sentencia");

				log_info(logs,"[HILO CPU] Se envio al CPU nro: %d la sentencia",socket);
				sem_post(&mutexOpera);
				break;
			case LEER_SEGMENTO:
				retardo();
				sem_wait(&mutexOpera);

				if(!recibirDato(socket,tam->length,(void*)&etiq,logs)){
					log_error(logs,"Se produjo un error recibiendo la esctructura");
					break;
				}

				if( validacion_base(etiq->base) ){
					log_error(logs,"[HILO CPU] no es correcta la base");
					break;
				}

				codigo = leer_segmento(etiq->base,etiq->tamanio,etiq->offset,pidLocal);

				tam->length = etiq->tamanio;
				enviarDatos(socket, tam, codigo, logs);
				log_info(logs,"[HILO CPU] Se envio el segmento al CPU nro: %d",socket);
				sem_post(&mutexOpera);
				break;
			default:
				if(!seBajoCPU)
					log_error(logs,"[HILO CPU] Se recibio una operacion invalida");
				termina = 1;
				break;
		}
		if(termina){
			log_error(logs,"[HILO CPU]La UMV desconectó a los CPU por fallo");
			break;
		}
	}
}

/*Funcion que llama el hilo cuando se conecta el KERNEL a la UMV*/
void funcion_kernel(int socket){
	log_info(logs,"[HILO KERNEL]Entra al hilo");
	int* pid;
	int pidLocal = -1;
	t_length* tam = malloc(sizeof(t_length));
	t_etiqueta* etiq = malloc(sizeof(t_etiqueta));
	datos_crearSeg* pidTam = malloc(sizeof(datos_crearSeg));
	t_elem_cola_kernel* unaPeticion = malloc(sizeof(t_elem_cola_kernel));
	segEscritos = 0;
	int kernelCortado=0;

	while(1){

		log_info(logs,"[HILO KERNEL] Esperando peticion...");
		if(!recibirMenu(socket, tam, logs)){
			log_error(logs, "[HILO KERNEL] Error al recibir la peticion");
			break;
		}

		unaPeticion->tama = tam;
		queue_push(colaKernel,unaPeticion);
		unaPeticion = queue_pop(colaKernel);
		kernelCortado=0;


		switch(unaPeticion->tama->menu){
			case PID_ACTUAL:
				retardo();
				sem_wait(&mutexOpera);
				if(!recibirDato(socket, tam->length, (void*)&pid, logs)){
					log_error(logs, "[HILO KERNEL] Se produjo un error recibiendo el pid");
					break;
				}
				pidLocal = *pid;
				log_debug(logs,"[HILO KERNEL] Se cambio el pid activo a %d", *pid);
				sem_post(&mutexOpera);
				break;
			case ESCRIBIR_SEGMENTO:
				retardo();
				sem_wait(&mutexOpera);
				segEscritos++;
				if(!recibirDato(socket,tam->length,(void*)&etiq,logs)){
					log_error(logs,"Se produjo un error recibiendo la esctructura");
					break;
				}

				log_debug(logs,"[H KERNEL] Datos recibidos para escribir un seg. de base: %d, offset: %d, tamanio: %d",etiq->base,etiq->offset,etiq->tamanio);

				if( validacion_base(etiq->base) ){
					log_error(logs,"[HILO KERNEL] Error en la base: %d al intentar escribir el segmento",etiq->base);
					break;
				}

				if(etiq->tamanio <= 0 || etiq->tamanio > 1024){
					log_error(logs,"[HILO KERNEL] El tam del seg a escribir es invalido. No se escribe el segmento. No entra al recv (del buffer para escribir)");
					break;
				}
				int base=etiq->base;
				int offset=etiq->offset;
				int tamanio=etiq->tamanio;
				char* buffer;
				if(!recibirDatos(socket,tam,(void*)&buffer,logs)){
					log_error(logs,"[HILO KERNEL] Se produjo un error recibiendo el codigo");
					break;
				}

				escribir_segmento(base,tamanio,offset,buffer,pidLocal);

				log_debug(logs,"Se escribio el segmento nro: %d",segEscritos);

				sem_post(&mutexOpera);
				sem_post(&yaEscribio);
				break;
			case CREAR_SEGMENTO:
				retardo();
				sem_wait(&mutexOpera);
				if(!recibirDato(socket, tam->length, (void*)&pidTam, logs)){
					log_error(logs, "Se produjo un error recibiendo pid-tamanio");
					break;
				}

				int baseLog = crear_agregar_segmento(pidTam->pid,pidTam->tamanio);

				tam->length = sizeof(int);
				enviarDatos(socket, tam, &baseLog, logs);
				sem_post(&mutexOpera);
				segEscritos=0;
				break;
			case ELIMINAR_SEGMENTOS:
				retardo();
				sem_wait(&mutexOpera);
				if(!recibirDato(socket, tam->length, (void*)&pid, logs)){
					log_error(logs, "Se produjo un error recibiendo el pid");
					break;
				}
				if(!destruir_segmentos(*pid)){
					break;
				}
				sem_post(&mutexOpera);
				break;
			case OK:
				log_error(logs,"Finalizo el kernel. La umv lo corta");
				kernelCortado=1;
				break;

			default:
				retardo();
				log_error(logs,"[HILO KERNEL] operacion invalida para Kernel.La UMV desconecta al KERNEL");
				break;
		}

		unaPeticion->tama->menu =OK;
		if(kernelCortado)
			break;

	}
	log_error(logs,"[HILO KERNEL]La UMV desconectó al kernel por fallo");
}


bool validacion_base(int base){
	if( base >= 0 && base <= 4000)
		return 0;
	return 1;
}

/* Funcion que solicita el bloque de memoria inicial para la UMV y
crea estructuras para el manejo de la UMV. */
void inicializar_umv(int tamanioUMV){

	tablaPidSeg = dictionary_create();
	listaHuecos = list_create();
	colaKernel = queue_create();
	colaCPUs = list_create();
	nodoHuecos* unElem= malloc(sizeof(nodoHuecos));
	unElem->dirFisica=malloc(tamanioUMV);
	unElem->tamanioSegmento=tamanioUMV;
	ramUMVInicial = unElem->dirFisica;
	list_add(listaHuecos,unElem);
	reporteEstructuras = txt_open_for_append("reporteEstructuras");
	resultadoConsola = txt_open_for_append("resultadoConsola");
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
	unElem->dirFisica = obtener_proxima_dirFisica(tamanio);
	if( string_equals_ignore_case(unElem->dirFisica,"-1") ){ //FIXME
		return -1;
	}
	if(dictionary_is_empty(tablaPidSeg) || !(dictionary_has_key(tablaPidSeg, pid))){
		unElem->idSegmento=1;
		listaSeg = list_create();
		list_add(listaSeg, (void*)unElem);
		dictionary_put(tablaPidSeg, pid, (void*)listaSeg);

		if(dictionary_has_key(tablaPidSeg,pid))
			log_info(logs,"Se agrego el segmento nro: %d del pid: %s",unElem->idSegmento,pid);
		else
			log_error(logs,"El nuevo campo pid/lista no se agregó al diccionario");
	}else if(dictionary_has_key(tablaPidSeg, pid)){
		listaSeg = dictionary_get(tablaPidSeg,pid);
		nroSeg = obtener_nuevo_nroSeg(listaSeg);
		unElem->idSegmento=nroSeg;
		list_add_in_index(listaSeg,0,(void*)unElem);
		if(nroSeg == (obtener_nuevo_nroSeg(listaSeg) - 1))
			log_info(logs,"Se creo el segmento nro: %d del pid:",nroSeg);
		else
			log_error(logs,"El nuevo segmento no se agregó a la lista correspondiente");
	}
	return unElem->dirLogica;
}

/* Funcion que elimina un campo pid y sus segmentos asociados */
bool destruir_segmentos(int pidInt){

	if( dictionary_has_key(tablaPidSeg, string_itoa(pidInt)) ){
		t_list* listaSeg = dictionary_get(tablaPidSeg,string_itoa(pidInt));
		list_iterate(listaSeg,(void*)elimina_segmento_agrega_hueco);
		dictionary_remove(tablaPidSeg,string_itoa(pidInt));

		if( !dictionary_has_key(tablaPidSeg, string_itoa(pidInt))){
			log_info(logs,"[HILO KERNEL]Los segmentos del pid: %d se eliminaron",pidInt);
			return 1;
		}else
			log_error(logs,"[HILO KERNEL]Los segmentos del pid: %d no se eliminaron",pidInt);
		return 0;
	}else{
		log_error(logs,"Se intento eliminar segmentos no existentes");
		return 0;
	}
}

/* Funcion que elimina un segmento y agrega un nodo a la lista de huecos */
void elimina_segmento_agrega_hueco(tablaSegUMV *unElem){
	nodoHuecos* nodoNuevo=malloc(sizeof(nodoHuecos));
	nodoNuevo->dirFisica = unElem->dirFisica;
	nodoNuevo->tamanioSegmento = unElem->tamanioSegmento;
	memcpy(nodoNuevo->dirFisica,"0000000",nodoNuevo->tamanioSegmento);
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
	switch(algoritmoActual){
		case FIRST_FIT:
//			log_debug(logs,"Algoritmo de asignación: First-Fit");
			return first_fit(tamanio);
		case WORST_FIT:
//			log_debug(logs,"Algoritmo de asignación: Worst-Fit");
			return worst_fit(tamanio);
	}
	return NULL;
}

/* Funcion que representa al funcionamiento del algoritmo first-fit de asignacion de memoria*/
char *first_fit(int tamanio){ //

	bool filtra_por_tamanio(nodoHuecos* unElem){
		return unElem->tamanioSegmento >= tamanio;
	}
	nodoHuecos* unElem = list_remove_by_condition(listaHuecos,(void*)filtra_por_tamanio);

	if(!unElem){
		compactar_memoria();
		unElem = list_remove_by_condition(listaHuecos,(void*)filtra_por_tamanio);
		if(!unElem){
			log_error(logs,"Memory overload");
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
		compactar_memoria();
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

	char* nuevaDirFisica = unElem->dirFisica;
//	char* nuevaDirFisica=malloc(sizeof(char)*tamanio);
//	memcpy(nuevaDirFisica,unElem->dirFisica,tamanio);
	unElem->dirFisica = unElem->dirFisica + tamanio;
	unElem->tamanioSegmento = unElem->tamanioSegmento - tamanio;
	list_add(listaHuecos,(void*)unElem);
	return nuevaDirFisica;
}

/*Funcion que lee un segmento de la umv y retorna un puntero a la posicion solicitada */
unsigned char *leer_segmento(int dirLog, int tamanioALeer, int offset, int pidAct){ //solicitar_memoria_desde_una_pos();

	bool buscar_dirLogica(tablaSegUMV* unElem){
		return dirLog == unElem->dirLogica;
	}
	if(dictionary_has_key(tablaPidSeg,string_itoa(pidAct))){
		t_list* listaSeg= dictionary_get(tablaPidSeg,string_itoa(pidAct));
		tablaSegUMV* unElem= list_find(listaSeg,(void*)buscar_dirLogica);
		if(unElem){
			if( unElem->dirFisica + offset + tamanioALeer <= unElem->dirFisica + unElem->tamanioSegmento ){
				unsigned char* destino = malloc(sizeof(char) * tamanioALeer); //FIXME memory leak
				char* desde = unElem->dirFisica + offset;
				memcpy(destino,desde,tamanioALeer);
				return destino;
			}else
				log_error(logs,"se esta tratando de leer fuera de los rangos del segmento-segmentation fault-");
		}else
			log_error(logs,"se intento acceder a una base inexistente");
	}
	else
		log_error(logs,"no existe el pid: %d al tratar de leer el seg",pidAct);
	return NULL;
}

/*Funcion que escribe el buffer en un segmento determinado*/
int escribir_segmento(int dirLog, int tamanioAEscribir, int offset, char* buffer, int pidAct){

	bool buscar_dirLogica(tablaSegUMV* unElem){
		return dirLog == unElem->dirLogica;
	}
	tablaSegUMV* unElem;

	t_list* listaSeg= dictionary_get(tablaPidSeg,string_itoa(pidAct));
	if(listaSeg){
		unElem= list_find(listaSeg,(void*)buscar_dirLogica);

		if(unElem){
			if(unElem->dirFisica + offset + tamanioAEscribir <= unElem->dirFisica + unElem->tamanioSegmento){
				char* espacioSegmento = unElem->dirFisica + offset;
				memcpy(espacioSegmento,buffer,tamanioAEscribir);
				return 0;

			}else{
				log_error(logs,"Se esta tratando de escribir fuera de los rangos del segmento");
				return -1;
			}
		}else
			log_error(logs,"La direccion logica no existe");
	}else
		log_error(logs,"no existe la lista asociada al pid");
	return 1;
}

void retardo(){
	if(retardoActual)
		sleep(retardoActual/1000);
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
void cambiar_pid_activo(int pid, int pidLocal){
	pidLocal = pid;
}

void imprime_listahuecos(){
	list_iterate(listaHuecos, (void*)imprime_campos_listahuecos);
}

void imprime_campos_listatablaSegUMV(tablaSegUMV *unElem){

	txt_write_in_file(reporteEstructuras,"\nNro segmento: ");
	txt_write_in_file(reporteEstructuras, string_itoa(unElem->idSegmento));
	txt_write_in_file(reporteEstructuras,"\n    Dirección logica:");
	txt_write_in_file(reporteEstructuras, string_itoa(unElem->dirLogica));

	txt_write_in_file(reporteEstructuras,"\n    Tamaño del segmento: ");
	txt_write_in_file(reporteEstructuras,string_itoa(unElem->tamanioSegmento));
	txt_write_in_file(reporteEstructuras,"\n    Contenido:\n");
	char* rta = malloc(unElem->tamanioSegmento);
	memcpy(rta,unElem->dirFisica,unElem->tamanioSegmento);
	txt_write_in_file(reporteEstructuras,rta);
	txt_write_in_file(reporteEstructuras,"\n");
	free(rta);
}

void imprime_campos_listahuecos(nodoHuecos *unElem){
	txt_write_in_file(reporteEstructuras,"\nContenido:\n");
	txt_write_in_file(reporteEstructuras,"  Tamaño del segmento:");
	txt_write_in_file(reporteEstructuras,string_itoa(unElem->tamanioSegmento));
	txt_write_in_file(reporteEstructuras,"\n");

}

void imprime_estructuras_memoria(char** operacion){
	void itera_diccionario(char* pid,t_list* listaSeg){
		list_iterate(listaSeg, (void*)imprime_campos_listatablaSegUMV);
	}
	if(!dictionary_size(tablaPidSeg))
		txt_write_in_file(reporteEstructuras,"**Actualmente no hay segmentos en memoria**\n\n");
	else{

		if(operacion[2] == 0)
			dictionary_iterator(tablaPidSeg,(void*)itera_diccionario);
		else{
			int pid=atoi(operacion[2]);
			t_list* listSeg = dictionary_remove(tablaPidSeg,string_itoa(pid));
			list_iterate(listSeg,(void*)imprime_campos_listatablaSegUMV);
		}
	}
}

void imprime_estado_mem_ppal(char** operacion){
	txt_write_in_file(reporteEstructuras,"			---ESTRUCTURAS DE MEMORIA---\n\n");
	imprime_estructuras_memoria(operacion);
	txt_write_in_file(reporteEstructuras,"\n			---BLOQUES LIBRES DE MEMORIA---\n");
	imprime_listahuecos();
}

void reporteContenidoMp(char **operacion){

//	int offset = atoi(operacion[2]);
//	int tam =  atoi(operacion[3]);
//	int base = ramUMVInicial; FIXME: esto es una incoherencia :)

	//FIXME: char* publicacion = leer_segmento(,tam,offset,);
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


void compactar_memoria(){
	log_info(logs,"entra a compactar memoria");
	ramAux = ramUMVInicial;
	tamHueco = 0;
	int yaDesplazo= 1;

	while(ramAux < ramUMVInicial + tamanioUMV){
		nodoHuecos* unElem = list_remove_by_condition(listaHuecos,(void*)buscar_ramAux_en_listaH);
		if(unElem){
			buscar_hueco_para_compactar(unElem);
			yaDesplazo=0;
		}else{
			if(!yaDesplazo){
				buscar_segmento_y_desplazarlo();
				tamHueco = 0;
				yaDesplazo=1;
			}else{
				buscar_el_primer_hueco_y_actualizar_ramAux();
			}
		}
		if(ramAux == ramUMVInicial + tamanioUMV){
			nodoHuecos* huecoFinal = malloc(sizeof(nodoHuecos));
			huecoFinal->dirFisica  = dirHueco;
			huecoFinal->tamanioSegmento = tamHueco;
			list_add(listaHuecos,huecoFinal);
		}

	}
}

//Funcion inherente a la compactacion
void buscar_hueco_para_compactar(nodoHuecos* unElem){
	dirHueco = unElem->dirFisica;
	do{
		tamHueco += unElem->tamanioSegmento;
		ramAux = unElem->dirFisica + unElem->tamanioSegmento;
		unElem = list_remove_by_condition(listaHuecos,(void*)buscar_ramAux_en_listaH);
	}while(unElem);
}

//Funcion inherente a la compactacion
void buscar_el_primer_hueco_y_actualizar_ramAux(){
	encontroHueco = 0;
	while(!encontroHueco){
		dictionary_iterator(tablaPidSeg,(void*)actualizar_RamAux);
	}
}

//Funcion inherente a la compactacion
void actualizar_RamAux(char* pid, t_list* listSeg){

	tablaSegUMV* unElem = list_find(listSeg,(void*)buscar_ramAux_endicc);
	if(unElem){
		ramAux = unElem->dirFisica + unElem->tamanioSegmento;
		encontroHueco = 1;
	}
}

//Funcion inherente a la compactacion
bool buscar_ramAux_endicc(tablaSegUMV* unElem){
	return unElem->dirFisica == ramAux;
}

//Funcion inherente a la compactacion
void buscar_segmento_y_desplazarlo(){
	dictionary_iterator(tablaPidSeg,(void*)busca_seg_en_diccionario);
}

//Funcion inherente a la compactacion
void busca_seg_en_diccionario(char* pid,t_list* listaSeg){
	nodoHuecos* nuevoElem = malloc(sizeof(nodoHuecos));
	tablaSegUMV* unElem = list_find(listaSeg,(void*)buscar_ramAux_endicc);

	if(tamHueco == unElem->tamanioSegmento){
		nuevoElem->dirFisica = unElem->dirFisica;
		nuevoElem->tamanioSegmento = unElem->tamanioSegmento;
		list_add(listaHuecos,nuevoElem);
	}else{
		nuevoElem->dirFisica = dirHueco + unElem->tamanioSegmento;
		nuevoElem->tamanioSegmento = tamHueco;
		list_add(listaHuecos,nuevoElem);
	}
	ramAux = dirHueco + unElem->tamanioSegmento;
	unElem->dirFisica = memmove(dirHueco,unElem->dirFisica,unElem->tamanioSegmento);
}


//Funcion inherente a la compactacion
bool buscar_ramAux_en_listaH(nodoHuecos* unElem){
	return unElem->dirFisica == ramAux;
}
