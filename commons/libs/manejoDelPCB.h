/*
 * manejoDelPCB.h
 *
 *  Created on: 18/04/2014
 *      Author: utnso
 */

#ifndef MANEJODELPCB_H_
#define MANEJODELPCB_H_

typedef struct registroPCB{
	int identificador_unico;
	int tamanio_contexto; //tamaño del contexto
	int program_counter; //indica que instruccion a ejecutar y la inicializa el parser
	//A partir de aca lo da el parser
	int primera_instruccion;
	int indice_etiquetas_funciones; //indice de etiquetas y funciones serializado
	int indice_codigo; //indice de codigo serializado
	int tamanioIndice_etiquetas_funciones; //tamaño del indice de etiquetas y funciones
	int tamanioIndice_codigo; //tamaño del indice del codigo
}registroPCB;

void inicializar();
void mostrar();

#endif /* MANEJODELPCB_H_ */
