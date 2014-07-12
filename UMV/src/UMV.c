/*
 ============================================================================
 Name        : UMV.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "functions.h"

int main(int argc, char** argv){

 	logs = log_create("log","UMV.c",1,LOG_LEVEL_TRACE);
	/*if (argc < 2){
		log_error(logs, "No se envio ningun parametro");
		log_destroy(logs);
		return 0;
	}*/
    config = config_create("config");
	if(!archivo_config_valido()){
		log_error(logs,"El archivo de configuracion no tiene todos los campos necesarios");
		log_destroy(logs);
		config_destroy(config);
		return 0;
	}
	inicializar_var_config();
	inicializar_umv(tamanioUMV);
	log_debug(logs,"Ya se creo la umv");

	//////////////////////

	log_debug(logs,"Levanto el hilo: Consola");
	pthread_create(&pthread_consola, NULL, (void*)consola, NULL);

	estructura_hilo* hilo = malloc(sizeof(estructura_hilo));
	t_length* tam = malloc(sizeof(t_length));
	int socket = crearServidor(puerto, logs);
	if(!socket)
		log_error(logs,"No se creo el servidor");
	else
		log_info(logs,"Se creo el servidor");

	log_debug(logs,"Entra al do-while para conectar el kernel");

	do{
		hilo->socket = aceptarConexion(socket, logs);
		if(!hilo->socket){
			log_error(logs,"Error al aceptar la conexion -kernel");
			log_destroy(logs);
			return 0;
		}else
			log_info(logs,"Se acepto la conexion -kernel-");

		if(!recibirMenu(hilo->socket, tam, logs))
			log_error(logs, "Se produjo un error haciendo el handshake");

		if (tam->menu == SOY_KERNEL){
			log_debug(logs,"Se conecto: Kernel");
			pthread_create(&pthread_kernel, NULL, (void*)funcion_kernel, hilo);
		}else
			log_error(logs, "Se esta esperando la conexion del kernel");

	}while(tam->menu != SOY_KERNEL);

	while(1){
		hilo->socket = aceptarConexion(socket, logs);
		if(!hilo->socket)
			log_error(logs,"Error al aceptar la conexion -cpu-");
		else
			log_info(logs,"Se acepto la conexion -cpu-");

		if(!recibirMenu(hilo->socket, tam, logs))
			log_error(logs, "Se produjo un error haciendo el handshake -cpu-");

		if (tam->menu == SOY_CPU){
			log_debug(logs,"Se conecto: CPU");
			pthread_create(&pthread_CPU, NULL, (void*)funcion_CPU, hilo);
		}else{
			log_error(logs, "Se conecto algo en la espera del CPU");
		}
	}
	pthread_join(pthread_consola,NULL);
	eliminarUMV();
	log_destroy(logs);
	config_destroy(config);
	return 0;
}
