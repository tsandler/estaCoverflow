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

t_intructions obtenerSiguienteIntruccion (registroPCB pcb){
	return pcb.indice_codigo[pcb.program_counter];
	//return *(&pcb.indice_codigo + sizeof(t_intructions) * pcb.program_counter); <-- esto es feo
}



registroPCB llenarPCB(registroPCB pcb, t_medatada_program program, int stack, int codigo){

		//SEGMENTO STACK
		pcb.cursor_stack = stack;
		pcb.tamanio_contexto = stack;
		pcb.tamanio_contexto = 0;

		//SEGMENTO DE CODIGO
		pcb.tamanio_indice_codigo = program.instrucciones_size; //numero de instrucciones
		pcb.program_counter = 0;
		pcb.segemento_codigo = codigo; //codigo literal
		pcb.indice_codigo = program.instrucciones_serializado; // intrucciones

		//SEGMENTO DE ETIQUETAS Y FUNCIONES
		pcb.tamanio_indice_etiquetas_funciones = program.etiquetas_size; //numero de instrucciones
		pcb.indice_etiquetas_funciones = program.etiquetas; //estructura "uno\0001dos\00002..."
		pcb.puntero_etiquetas_funciones = 0;


	return pcb;
}

