<<<<<<< HEAD
=======
/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/temporal.h>

int main(void) {
	//inicializar();
	//mostrar();
	char *tiempo = temporal_get_string_time();
	puts (tiempo);
	free (tiempo);
	return 0;
}

>>>>>>> 3270cb850a1a4931bd8961d47cbf4efa307283c2
