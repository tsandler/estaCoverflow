/*
 * manejoDelProcesoPrograma.h
 *
 *  Created on: 19/04/2014
 *      Author: utnso
 */

#ifndef MANEJODELPROCESOPROGRAMA_H_
#define MANEJODELPROCESOPROGRAMA_H_


//(pagina 5 del pdf)
typedef struct registroProceso{
	char parametro_acceso[20];
	int valor_numerico; //valor para sincronizarlo con sus respectivos procesos
	char valor_parametro[15];
	struct registroProceso *sig;
}registroProceso;

void imprimirTexto(); //recibe por parametro el nombre del archivo
void imprimir();

#endif /* MANEJODELPROCESOPROGRAMA_H_ */
