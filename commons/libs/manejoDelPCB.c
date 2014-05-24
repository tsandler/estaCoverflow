/*
 * manejoDelPCB.c
 *
 *  Created on: 18/04/2014
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include "../parser/metadata_program.h"
#include "manejoDelPCB.h"

registroPCB crearNuevoPCB (){

	registroPCB pcb;

	//INICIALIZO LAS VARIABLES DEL NUEVO PCB
	pcb.pid = identificadorUnicoDelPCB();

	return pcb;
}




int calcularPesoPCB (t_medatada_program program ){

	int peso=0;


	peso = program.cantidad_de_etiquetas + (3 * program.cantidad_de_funciones) + program.instrucciones_size;

	return peso;
}

registroPCB llenarPCB(registroPCB pcb, t_medatada_program program, int stack, int codigo){



		//SEGMENTO STACK
		//Deberia igualarlo a la posicion donde comienza el stack, pero nunca se mueve
		pcb.cursor_stack = stack;

		//Este puntero recorre el segmento del stack.
		pcb.tamanio_contexto = stack;
		pcb.tamanio_contexto = 0;

		//SEGMENTO DE CODIGO
		 //Numero de intrucciones
		pcb.tamanio_indice_codigo = program.instrucciones_size;

		pcb.program_counter = 0;

		//Puntero a primera intruccion a ejecutar en el segmento. guarda el
		//el inicio del segmento de Codigo
		pcb.segemento_codigo = codigo;
		pcb.indece_codigo = codigo;

		//SEGMENTO DE ETIQUETAS, FUNCIONES Y PROCEDIMEINTO
		// aca dijo alien que debo usar una funcion metadata loca que me
		//devuelve el lugar del indice de etiquetas. eh, pero al pcb no
		//le importa, en generar.

		//Pido la memoria necesaria para craer el vector de intruccion a ejecutar
		pcb.tamanio_indice_etiquetas_funciones = program.etiquetas_size;
		pcb.indice_etiquetas_funciones = program.etiquetas;
		pcb.puntero_etiquetas_funciones = 0;

		//HEAP


	return pcb;
}

