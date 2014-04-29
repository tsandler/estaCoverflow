int main(){	
	t_log *logs = log_create("log", "CPU.c", 0, LOG_LEVEL_TRACE);
	t_config *config = config_create("config.cnf");

	int port = config_get_int_value(config, "PORT");
	int bytesRecibidos;
	char buffer[BUFF_SIZE ]; //constante que por ahora vale 1024, dsps vemos el tema del tamano

	int socket_conectado = crearServidor(port, logs);

	if(socket_conectado < 0 ){
		log_error(logs, "El servidor no se creo correctamente");
		perror("El proceso no se realizo correctamente.");
		return 0;
	}

	while (1){
		bytesRecibidos=recv(socket_conectado, buffer, 1024, 0); //buffer es la variable que recibe
		if(bytesRecibidos >= 0){
			printf("Recibio: %s\n", buffer);
			if (memcmp(buffer, "fin", bytesRecibidos) == 0){
				break;
			}
		}else{
			printf("Hubo un error");
			goto endProgram;
		}
	}

	log_info(logs, "El proceso se realizo correctamente");
	puts("El proceso se realizo correctamente.");

	//En caso de que haya habido error salto a esta instruccion para evitar que se
	//imprima el mensaje de proceso realizado, no hay que abusar del uso del goto.
	endProgram:
	log_destroy(logs);
	config_destroy(config);
	close(socket_conectado);
	return 0;
}
