/*
 * testeo.h
 *
 *  Created on: 26/05/2014
 *      Author: utnso
 */

#ifndef TESTEO_H_
#define TESTEO_H_

#include "globales.h"

void imprime_listatablaSegUMV(t_list* lista);

// TESTS QUE ANDAN
void test_crea_elimina_UMV();
void test_crear_agregar_segmento(int pid, int tamanio);
void test_first_fit();
void test_crear_varios_segmentos_y_eliminarlos();

void test_crear_varios_segs_y_eliminar_uno_para_compactar();
//void test_crear_varios_segmentos_y_llamar_a_leer_segmento();

#endif /* TESTEO_H_ */
