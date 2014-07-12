/*
 * functions.h
 *
 *  Created on: 16/05/2014
 *      Author: utnso
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "globales.h"

void inicializar_umv(int tamanioUMV);
void eliminarUMV();

int crear_agregar_segmento(int pid, int tamanio);
void destruir_segmentos(int pidInt);

unsigned char *leer_segmento(int dirLog, int tamanioALeer, int offset);
void escribir_segmento(int dirLog, int tamanioAEscribir, int offset, char* buffer);

int obtener_proxima_dir_logica(int tamanio, char* pid);
char *obtener_proxima_dirFisica(int tamanio);
int obtener_nuevo_nroSeg(t_list *listaSeg);

bool sort_nroSeg(tablaSegUMV* unElem, tablaSegUMV* otroElem);
bool sort_mayor_hueco(nodoHuecos* unElem, nodoHuecos* otroElem);
bool sort_mayor_dirLogica(tablaSegUMV* unElem, tablaSegUMV* otroElem);
void elimina_segmento_agrega_hueco(tablaSegUMV *unElem);

char *first_fit(int tamanio);
char *worst_fit(int tamanio);
char *manejo_memoria(nodoHuecos* unElem, int tamanio);

void consola();
unsigned char* ejec_operacion(int nroOp);
void retardo();
void cambiar_retardo(int retardoNuevo);
void cambiar_pid_activo(int pid);
void cambiarAlgoritmo(int cambioAlgoritmo);
void dump();

void imprime_listahuecos();
void imprime_campos_listatablaSegUMV(tablaSegUMV *unElem);
void imprime_campos_listahuecos(nodoHuecos *unElem);
void imprime_estado_mem_ppal();
void imprime_estructuras_memoria();

bool archivo_config_valido();
void inicializar_var_config();
void vaciarLista(t_list* listaSeg);
//void eliminar_campos_listSeg(tablaSegUMV* unElem);
//void eliminar_campos_listHuecos(nodoHuecos* unElem);

void generar_archivo(unsigned char* resultado, int nroOp);

void funcion_kernel(int socket);
void funcion_CPU(int socket);

void compactar_memoria();
void buscar_segmento_y_desplazarlo();
bool buscar_ramAux(tablaSegUMV* unElem);
void busca_seg_en_diccionario(char* pid,t_list* listaSeg);



#endif /* FUNCTIONS_H_ */
