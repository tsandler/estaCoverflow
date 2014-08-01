/*
 * functions.h
 *
 *  Created on: 16/05/2014
 *      Author: utnso
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "globales.h"

void funcion_kernel(int socket);
void funcion_CPU(int socket);
t_elem_cola_cpu *desencolar_peticion(int socket);

void inicializar_umv(int tamanioUMV);
void eliminarUMV();

int crear_agregar_segmento(int pid, int tamanio);
bool destruir_segmentos(int pidInt);

unsigned char *leer_segmento(int dirLog, int tamanioALeer, int offset, int pidLocal);
int escribir_segmento(int dirLog, int tamanioAEscribir, int offset, char* buffer, int pidLocal);

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


void retardo();
void cambiar_retardo(int retardoNuevo);
void cambiar_pid_activo(int pid, int pidLocal);
void cambiarAlgoritmo(int cambioAlgoritmo);

void imprime_listahuecos();
void imprime_campos_listatablaSegUMV(tablaSegUMV *unElem);
void imprime_campos_listahuecos(nodoHuecos *unElem);
void imprime_estructuras_memoria();
void reporteContenidoMp(char **operacion);
void imprime_estado_mem_ppal(char** operacion);

bool archivo_config_valido();
void inicializar_var_config();
void vaciarLista(t_list* listaSeg);


void compactar_memoria();
void buscar_segmento_y_desplazarlo();
void busca_seg_en_diccionario(char* pid,t_list* listaSeg);
bool buscar_ramAux_en_listaH(nodoHuecos* unElem);
bool buscar_ramAux_endicc(tablaSegUMV* unElem);
void buscar_hueco_para_compactar(nodoHuecos* unElem);
void buscar_el_primer_hueco_y_actualizar_ramAux();
void actualizar_RamAux(char* pid, t_list* listSeg);

bool validacion_base(int base);
bool esCorrecto_pid(int pid);
void bajar_cpu(int socket);


#endif /* FUNCTIONS_H_ */
