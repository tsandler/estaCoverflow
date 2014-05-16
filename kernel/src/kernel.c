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
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <parser/metadata_program.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <libs/sockets.h>

//ESTRUCTURAS

typedef struct {
	t_medatada_program medatadaprogram;
	int peso;
} t_medatada_program_con_prioridad;
typedef struct{
	t_log *logs;
	t_config *config;
	int socket_conectado;
	int bytesRecibidos;
	char buffer[1024 ];
	int error;
	t_medatada_program programa;
}PLP;

//DECLARACION DE FUNCIONES
//void manejar_cola_ready (t_list *colaReady , t_medatada_program programaNuevo);
//int calcular_peso(t_medatada_program programa);
//t_medatada_program_con_prioridad obtener_programa_con_peso(t_medatada_program programaNuevo, t_medatada_program_con_prioridad programaConPeso);
void *programador_largo_plazo(void* auxPLP);
//bool comparador (void *, void *);
void *programador_corto_plazo(void*);

//FUNCION PRINCIPAL


int main() {

	t_log *logs = log_create("log", "kernel.c", 0, LOG_LEVEL_TRACE);
	t_config *config = config_create("config.cnf");
	int port = config_get_int_value(config, "PUERTO_PROGRAM");
	int socket_conectado = crearServidor(port, logs);
	pthread_t thr1;//, thr2;
	PLP auxPLP;
	// char *PCP = "PCP";
	int r1;//, r2;

	if(socket_conectado < 0 ){
		log_error(logs, "El servidor no se creo correctamente");
		perror("El proceso no se realizo correctamente.");
		config_destroy(config);
		log_destroy(logs);

	}else{
		r1 = pthread_create( &thr1, NULL, programador_largo_plazo, (void*) &auxPLP);
		// r2 = pthread_create( &thr2, NULL, programador_corto_plazo, (void*) PCP);};


	};
	return 0;
};

