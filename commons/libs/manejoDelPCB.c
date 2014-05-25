/*
 * manejoDelPCB.c
 *
 *  Created on: 24/05/2014
 *      Author: utnso
 */


#include "manejoDelPCB.h"

int identificadorUnicoDelPCB(){
	ultimoIdentificador =+ 1;
	return ultimoIdentificador;
}

registroPCB crearNuevoPCB (){

	registroPCB pcb;
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
		pcb.cursor_stack = stack;
		pcb.tamanio_contexto = stack;
		pcb.tamanio_contexto = 0;

		pcb.tamanio_indice_codigo = program.instrucciones_size;
		pcb.program_counter = 0;
		pcb.segemento_codigo = codigo;
		pcb.indice_codigo = program.instrucciones_serializado;

		pcb.tamanio_indice_etiquetas_funciones = program.etiquetas_size;
		pcb.indice_etiquetas_funciones = program.etiquetas;
		pcb.puntero_etiquetas_funciones = 0;

	return pcb;
}
