/*
 * functions.h
 *
 *  Created on: 16/05/2014
 *      Author: utnso
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "globales.h"

bool criterio(nodoDirLog *unElem);
void inicializar_umv(int tamanioUMV);
void eliminarUMV();
int obtener_nroSeg(t_list *listaSeg);
bool sort_nroSeg(tablaSegUMV* unElem, tablaSegUMV* otroElem);
char *obtener_dir_fisica(int tamanio,int dirLogica);
int obtener_dir_logica(int tamanio);

#endif /* FUNCTIONS_H_ */
