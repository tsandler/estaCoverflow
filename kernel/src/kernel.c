/*
 ============================================================================
 Name        : kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <libs/manejoDelPCB.h>
#include <commons/string.h>
<<<<<<< HEAD



int main(void) {
 printf("%s", string_repeat ('a',5));

	return EXIT_SUCCESS;
=======

int main(void) {
	inicializar();
	mostrar();
	//char *tiempo = string_repeat('a', 5);
	//puts("DD");
	//puts (tiempo);
	//free (tiempo);
	return 0;
>>>>>>> 3270cb850a1a4931bd8961d47cbf4efa307283c2
}
