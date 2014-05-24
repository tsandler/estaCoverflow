/*
 * manejoDelPCB.h
 *
 *  Created on: 18/04/2014
 *      Author: utnso
 */

#ifndef MANEJODELPCB_H_
#define MANEJODELPCB_H_

int ultimoIdentificador = 0;

int identificadorUnicoDelPCB()
{
	ultimoIdentificador =+ 1;
	return ultimoIdentificador;
}


typedef struct registroPCB{
    int pid;

	//SEGMENTO DEL STACK
	int segemento_stack; // puntero que siemrpe apunta al inicio del segmento del stack
	int cursor_stack; // es el puntero que recorre el stack
	int tamanio_contexto; //es un valor calculable en base a la diferencia del puntero del stack y contexto.

	//SEGMENTO DE CODIGO
	int segemento_codigo; //puntero que apunta simepre al inicio del segemento del codigo.
	int indece_codigo; //  puntero que se mueve en el segmento codigo.
	int tamanio_indice_codigo ; //tamaño del indice del codigo
	t_intructions* instrucciones;	// array que guarda en cada componenete [longitud, desplazamiento]
	int program_counter; //recorre el arrar del indice de codigo.

	//SEGMENTO DE ETIQUETA, FUNCIONES Y PROCEDIMIENTOS

	char* indice_etiquetas_funciones; //indice de etiquetas y funciones serializado
	int puntero_etiquetas_funciones;
	int tamanio_indice_etiquetas_funciones; //tamaño del indice de etiquetas y funciones

	//HEAP....
	int puntero_heap;



}registroPCB;

registroPCB crearNuevoPCB();
registroPCB llenarPCB();
void inicializar();
void mostrar();

#endif /* MANEJODELPCB_H_ */
