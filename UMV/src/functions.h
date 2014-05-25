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

char *obtener_dir_fisica(int tamanio,int dirLogica);
int obtener_dir_logica(int tamanio);
int obtener_nroSeg(t_list *listaSeg);
bool sort_nroSeg(tablaSegUMV* unElem, tablaSegUMV* otroElem);

bool criterio(nodoDirLog *unElem);
bool criterio_remover(nodoDirLog *elemLista);
void criterio_iterate(tablaSegUMV *unElem);



#endif /* FUNCTIONS_H_ */
