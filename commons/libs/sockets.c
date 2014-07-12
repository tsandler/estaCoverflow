/*
 * sockets.c
 *
 *  Created on: 23/04/2014
 *      Author: utnso
 */

#include "sockets.h"

/* Funcion para crear el servidor y procesar los datos que son enviados
por el cliente, devuelve -1 en caso de error. */

int crearServidor(int port, t_log *logs){
	int s;
	struct sockaddr_in dir;

	dir.sin_family = PF_INET;
	dir.sin_port = htons(port);
	dir.sin_addr.s_addr = htonl(INADDR_ANY);

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s < 0){
		log_error(logs, "[SOCKETS] No se pudo conectar con el socket.");
		return -1;
	}

	if (bind(s, (struct sockaddr*)&dir, sizeof(dir)) != 0){
		log_error(logs, "[SOCKETS] Error al bindear el socket.");
		close(s);
		return -1;
	}

	if (listen(s, SOMAXCONN) != 0) {
		log_error(logs, "[SOCKETS] Error al escuchar el socket.");
		close(s);
		return -1;
	}
	return s;
}

/*Funcion para aceptar conexiones entrantes (una vez que el servidor fue creado),
  devuelve -1 en caso de error */

int aceptarConexion(int socket, t_log *logs){
	int socket_conectado;
	if ((socket_conectado = accept(socket, NULL, 0)) < 0) {
		log_error(logs, "[SOCKETS] Error al aceptar conexion entrante");
		return -1;
	}
	return socket_conectado;
}

/* Funcion para conectar al cliente con el servidor y enviar datos, recibe los
   datos de conexion, devuelve 0 en caso de error */

int conectarCliente(char *ip, int port, t_log *logs){
	int s;
	struct sockaddr_in dir;

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s < 0){
		log_error(logs, "[SOCKETS] No se pudo crear el socket");
		return -1;
	}
	dir.sin_family = PF_INET;
	dir.sin_port = htons(port);
	dir.sin_addr.s_addr = inet_addr(ip);
	if (connect(s, (struct sockaddr*) &dir, sizeof(dir))!= 0) {
		log_error(logs, "[SOCKETS] Error al conectar socket");
		close(s);
		return -1;
	}

	return s;
}

/*Recibe como parametro la estructura que indica el tamanio a enviar y los datos,
  enviando primero uno y despues el otro, devuelve 0 en caso de error.
  El socket que recibe es el devuelto por aceptarConexion() o conectarCliente() */

int enviarDatos(int socket, t_length* tam, void* datos, t_log* logs){
	if (send (socket, tam, sizeof(t_length), 0) < 0){
		log_error(logs, "[SOCKETS] Se produjo un problema al enviar el tamanio del dato");
		return 0;
	}
	if (send (socket, datos, tam->length, 0) < 0){
		log_error(logs, "[SOCKETS] Se produjo un problema al enviar el dato");
		return 0;
	}
	return 1;
}

/* Enviar una senal con un menu de lo que se va a realizar con la conexion */

int enviarMenu(int socket, t_length* tam, t_log* logs){
	if (send (socket, tam, sizeof(t_length), 0) < 0){
		log_error(logs, "[SOCKETS] Se produjo un problema al enviar el menu");
		return 0;
	}
	return 1;
}

/* Recibe la senal enviada por sockets con el tamanio del dato que se va a procesar y un menu */

int recibirMenu(int socket, t_length* tam, t_log* logs){
	if (recv (socket, tam, sizeof(t_length), MSG_WAITALL) < 0){
		log_error(logs, "[SOCKETS] Se produjo un problema al recibir el tamanio del dato");
		return 0;
	}
	return 1;
}

/* Recibe un dato del tamanio especificado por parametro */

int recibirDato(int socket, int size, void** dato, t_log* logs){
	*dato = malloc(size);
	if (recv (socket, dato, size, MSG_WAITALL) < 0){
		log_error(logs, "[SOCKETS] Se produjo un problema al recibir el tamanio del dato");
		return 0;
	}
	return 1;
}

/* Recibe en los parametros un t_lenght tamanio y la variable que va a almacenar
   los datos recibidos, devuelve 0 en caso de error.
   El socket que recibe es el devuelto por aceptarConexion() o conectarCliente() */

int recibirDatos(int socket, t_length* tam, void** datos, t_log* logs){
	if (recv (socket, tam, sizeof(t_length), MSG_WAITALL) < 0){
		log_error(logs, "[SOCKETS] Se produjo un problema al recibir el tamanio del dato");
		return 0;
	}
	*datos = malloc(tam->length);
	if (recv (socket, datos, tam->length, MSG_WAITALL) < 0){
		log_error(logs, "[SOCKETS] Se produjo un problema al recibir el dato");
		return 0;
	}
	return 1;
}

/* Cierra el socket enviado */

void cerrarSocket(int socket){
	close (socket);
}
