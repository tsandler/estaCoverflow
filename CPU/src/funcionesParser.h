/*
 * funcionesParser.h
 *
 *  Created on: 18/05/2014
 *      Author: utnso
 */

#ifndef FUNCIONESPARSER_H_
#define FUNCIONESPARSER_H_

#include "globales.h"
#include <parser/metadata_program.h>
#include <commons/string.h>

void inicializar_funciones_parser();

t_puntero definir_variable(t_nombre_variable identificador_variable);
t_puntero obtener_posicion_variable(t_nombre_variable identificador_variable);
t_valor_variable dereferenciar(t_puntero direccion_variable);
void asignar(t_puntero direccion_variable, t_valor_variable valor );
t_valor_variable obtener_valor_compartida(t_nombre_compartida variable);
t_valor_variable asignar_valor_compartida(t_nombre_compartida variable, t_valor_variable valor);
void ir_al_label(t_nombre_etiqueta etiqueta);
void llamar_sin_retorno(t_nombre_etiqueta etiqueta);
void llamar_con_retorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar);
void finalizar();
void retornar(t_valor_variable retorno);
void imprimir(t_valor_variable valor_mostrar);
void imprimir_texto(char* texto);
void entrada_salida(t_nombre_dispositivo dispositivo, int tiempo);
void wait(t_nombre_semaforo identificador_semaforo);
void signal_parser(t_nombre_semaforo identificador_semaforo);

#endif /* FUNCIONESPARSER_H_ */
