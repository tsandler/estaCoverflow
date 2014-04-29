int main(){	
	t_log *logs = log_create("log", "program.c", 0, LOG_LEVEL_TRACE);
	t_config *config = config_create("config.cnf");

	char *ip = config_get_string_value(config, "IP");
	int port = config_get_int_value(config, "PORT");
	int s;
	char *buffer;

	s = conectarCliente(ip, port, logs);

	if (s < 0){
		log_error(logs, "El cliente no se pudo conectar correctamente");
		perror("El cliente no se pudo conectar");
		log_destroy(logs);
		config_destroy(config);
		return 0;
	}
	scanf("%s", buffer); //buffer es la variable a enviar
	while (1) {
		if (send(s, buffer, strlen(buffer) + 1, 0) >= 0){
			printf("Datos enviados!\n");
				if (strcmp(buffer, "fin") == 0) {
				log_info(logs, "Cliente cerrado correctamente.");
				printf("Cliente cerrado correctamente.\n");
				break;
			}
		} else {
			log_error(logs, "Error al enviar datos, server no encontrado.");
			perror("Error al enviar datos. Server no encontrado.\n");
			goto endProgram;
		}
		scanf("%s", buffer);
	}

	log_info(logs, "El cliente se conecto y envio la informacion correctamente");
	puts("El cliente envio la informacion correctamente.");
	
	//En caso de que haya habido error salto a esta instruccion para evitar que se
	//imprima el mensaje de proceso realizado, no hay que abusar del uso del goto.
	endProgram:
	config_destroy(config);
	log_destroy(logs);
	close(s); 
	return 0;
}
