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
	  logs= log_create("log","UMV.c",0,LOG_LEVEL_TRACE);
	   if (argc < 2){
		   log_error(logs, "No se envio ningun parametro");
		   log_destroy(logs);
	   	   }

	   return 0;

	    t_config *config = config_create(argv[1]);
	    tamanioUMV = config_get_int_value(config, "tamanioUMV");


	    log_destroy(logs);
	    config_destroy(config);
	    return 0;

	      	}
