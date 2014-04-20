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
	char variable; //No sabemos si puede ser un string
	int8_t valor; /* el stack va a tener registros, pero no estamos seguros que
	el stack sea una estructura de registros */
}registroStack;

#endif /* MANEJOSTACK_H_ */
