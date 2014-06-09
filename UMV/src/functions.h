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

char *leer_segmento(int dirLog, int tamanioALeer, int offset);
void escribir_segmento(int dirLog, int tamanioAEscribir, int offset, void* buffer);

int obtener_proxima_dir_logica(int tamanio, char* pid);
char *obtener_proxima_dirFisica(int tamanio);
int obtener_nuevo_nroSeg(t_list *listaSeg);

bool sort_nroSeg(tablaSegUMV* unElem, tablaSegUMV* otroElem);
bool sort_mayor_hueco(nodoHuecos* unElem, nodoHuecos* otroElem);
bool sort_mayor_dirLogica(tablaSegUMV* unElem, tablaSegUMV* otroElem);
void criterio_iterate(tablaSegUMV *unElem);

nodoHuecos *first_fit(int tamanio);
nodoHuecos *worst_fit();

void retardo();
void cambiar_retardo(int retardoNuevo);
void cambio_proceso_activo(int pid);
char* ejec_operacion(int nroOp);
void cambiarAlgoritmo(int cambioAlgoritmo);
void dump();

void imprime_listahuecos();
void imprime_campos_listatablaSegUMV(tablaSegUMV *unElem);
void imprime_campos_listahuecos(nodoHuecos *unElem);
void imprime_estado_mem_ppal();
void imprime_estructuras_memoria();

#endif /* FUNCTIONS_H_ */
