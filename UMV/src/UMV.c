/*
 ============================================================================
 Name        : UMV.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <libs/manejoStack.h>
#include <commons/string.h>

int main(void) {
	//inicializar();
	//mostrar();
	char *tiempo = string_repeat('a', 5);
	puts (tiempo);
	free (tiempo);
	return 0;
}
