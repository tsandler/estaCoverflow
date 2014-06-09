/*programador_largo_plazo.c

  Created on: 12/05/2014
     Author: utnso
*/

#include "globales.h"
#include <parser/metadata_program.h>
#include <libs/manejoDelPCB.h>


typedef struct registroPCB_conPeso{registroPCB PCBprograma;
								   int peso;}registroPCB_conPeso;

bool comparador (registroPCB_conPeso *unElem, registroPCB_conPeso *otroElem);
int calcularPesoPCB (t_metadata_program programa);
void enviarProcesoUMV(registroPCB PCBprograma, t_log *logs, int socket_UMV);
void conectarseUMV(registroPCB PCBprograma , PLP* varPLP);

void planificador_largo_plazo(PLP* varPLP){
	t_metadata_program programa;
	registroPCB PCB_programa;
	registroPCB_conPeso* PCBprograma_conPeso;
	int peso_PCB;
	int stack = 0;
	int codigo = 0;
	//static pthread_mutex_t semaforo_mutex = PTHREAD_MUTEX_INITIALIZER; //creo el semaforo mutex que me
									//permitira que no ingresen dos procesos a la vez a la cola ready


	peso_PCB = calcularPesoPCB(programa);//calculo el peso del programa para insertarlo ordenado en la lista
	PCB_programa = crearNuevoPCB(programa);//creo la estructura PCB del programa recibido
	PCB_programa = llenarPCB(PCB_programa, programa, stack, codigo);//cargo los datos a la estructura de PCB creada
	PCBprograma_conPeso->PCBprograma = PCB_programa; //Completo la estructura para poner en la colaNew
	PCBprograma_conPeso->peso = peso_PCB;//Termino de agregar el peso para poner en la colaNew
	list_add(colaNew,&PCBprograma_conPeso);//agrego la estructura del programa a la colaNew
	printf("Se agrego el programa %i en la cola New", &PCBprograma_conPeso->PCBprograma.pid);
	list_sort(colaNew,(void*)comparador);//ordeno la colaNew por eso cree la estructura PCB_conPeso


	if (sem_wait (sem_multiprogramacion) == -1) //comparo que el semaforo no este bloqueado
	{perror("Semaforo invalido");               // luego le bajo el grado de multiprogramacion
	exit(1);}
	else{
	PCBprograma_conPeso = list_remove(colaNew,0); //lo saco de la colaNew al PCB
	printf("El programa %i dejo de estar en la cola New" , &PCBprograma_conPeso->PCBprograma.pid);
	pthread_mutex_lock(&semaforo_mutex);	//Bloqueo la cola para que se pueda ingresar nada
	list_add(colaReady,&PCBprograma_conPeso->PCBprograma);//mando el PCB a la colaReady y me olvido del peso debido a que trabaja con RR
	printf("El programa %i ingresa a la cola Ready",&PCBprograma_conPeso->PCBprograma.pid);
	pthread_mutex_unlock(&semaforo_mutex); //Desbloque la cola para que se puedan seguir ingresando cosas
	conectarseUMV(PCBprograma_conPeso->PCBprograma ,varPLP);// se realiza todo lo necesario para conectarse con la UMV
		}
}


bool comparador (registroPCB_conPeso *unElem, registroPCB_conPeso *otroElem){
	return unElem->peso > otroElem->peso;
};
void enviarProcesoUMV(registroPCB PCBprograma,t_log *logs, int socket_UMV){
	if (enviarDatos(socket_UMV, tam, &PCBprograma, logs))
		//envio los datos del PCB a la UMV
			log_info(logs, "El PCB fue enviado");
		else
			log_error(logs, "Se produjo un error enviando el PCB");
};
void conectarseUMV(registroPCB PCBprograma , PLP* varPLP){
    int socket_UMV;
    char *ip = config_get_string_value(varPLP->config,"IP");
    int port = config_get_int_value(varPLP->config,"PUERTO_UMV");
    socket_UMV = conectarCliente(ip, port, varPLP->logs);
	if(socket_UMV < 0){
		log_error(varPLP->logs, "El cliente no se pudo conectar correctamente");
		log_destroy(varPLP->logs);
	}else{
		log_info(varPLP->logs, "El kernel se conecto correctamente con la UMV");
		enviarProcesoUMV(PCBprograma, varPLP->logs, socket_UMV);
		cerrarSocket(socket_UMV);
	};}

