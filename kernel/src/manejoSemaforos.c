/*
 * manejoSemaforos.c
 *
 *  Created on: 27/06/2014
 *      Author: utnso
 */
#include "globales.h"
#include <sched.h>
#include <fcntl.h>
#include <unistd.h>


extern t_config *config;
extern t_log *logs;

char** nombre_semaforos;
char** valores_semaforos;

// creo todos los semaforos enviados por archivo de configuracion

void crearSemaforos(char** nombre_semaforos, char** valores_semaforos){
	nombre_semaforos = config_get_array_value(config,"SEMAFOROS");
	valores_semaforos = config_get_array_value(config,"VALOR_SEMAFORO");
	int cantidad_nombres_sem;
	int cantidad_valores_sem;
	cantidad_nombres_sem =sizeof(nombre_semaforos);
	cantidad_valores_sem =sizeof(valores_semaforos);
	int i;
	int valor_semaforo;
	sem_t *sem_cont;
	char nombre_semaforo;

	if (cantidad_nombres_sem == cantidad_valores_sem){
		for(i = 0 ; i<= cantidad_nombres_sem ;i++ ){
			if (nombre_semaforo != '\0'){
				valor_semaforo = *valores_semaforos[i];
				nombre_semaforo = *nombre_semaforos[i];
				char* nombre_sem = string_from_format("\\" , nombre_semaforo);
				if((sem_cont  = sem_open(nombre_sem, O_CREAT ,0644, valor_semaforo)) == (sem_t*)-1){
					log_error(logs,"No se pudo crear el semaforo %s",nombre_semaforo);
					perror ("No se puede crear el semaforo");
					exit(EXIT_FAILURE);//exit(1);
				}else{
					log_info(logs,"Se Creo el semaforo %s con un valor de %i",nombre_semaforo,valor_semaforo); //hay que ver si anda
				};
			};
		}
	}else{
		log_error(logs,"La cantidad de nombres y de valores de semaforos son distintas");
		perror ("La cantidad de nombres de semaforos es distinta a la cantidad de valores de semaforos" );
		exit(EXIT_FAILURE);//exit(1);

	}
}


