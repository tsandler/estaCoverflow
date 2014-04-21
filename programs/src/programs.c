/*
 ============================================================================
 Name        : programs.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
<<<<<<< HEAD
#include "headers/manejoDelProcesoPrograma.h"
=======
#include <commons/temporal.h>
>>>>>>> 3270cb850a1a4931bd8961d47cbf4efa307283c2

int main(void) {
	//inicializar();
	//mostrar();
	char *tiempo = temporal_get_string_time();
	puts (tiempo);
	free (tiempo);
	return 0;
}
