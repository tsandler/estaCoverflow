/*
 * manejoStack.h
 *
 *  Created on: 19/04/2014
 *      Author: utnso
 */

#ifndef MANEJOSTACK_H_
#define MANEJOSTACK_H_

#include <stdint.h>

typedef struct registroStack{
	char variable1; //No sabemos si puede ser un string
	char variable2; /* el stack va a tener registros, pero no estamos seguros que
	el stack sea una estructura de registros (Decidimos hacerlo de esta manera para
	poder sumar direcciones de memoriaa)*/
}registroStack;

#endif /* MANEJOSTACK_H_ */
