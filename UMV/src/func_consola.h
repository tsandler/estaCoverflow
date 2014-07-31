/*
 * func_consola.h
 *
 *  Created on: 30/07/2014
 *      Author: utnso
 */

#ifndef FUNC_CONSOLA_H_
#define FUNC_CONSOLA_H_

#include "functions.h"

#define OPERACION 2120
#define RETARDO 1298
#define ALGORITMO 2055
#define COMPACTAR 3835
#define DUMP 343
#define MAX 9
#define CREAR 4316
#define ALMACENAR 4907
#define SOLICITAR 5340
#define DESTRUIR 7167
#define HELP 322

int cadenaEntera(char s[]);
char *crearSegmentoPorConsola(char **operacion);
char* destruirSegmentosPorConsola(char** operacion);
char *solicitarBytesPorConsola(char **operacion);
char *almacenarBytesPorConsola(char **operacion,char *cadena);
int contadorEspacios(char *frase);
int cantidadDeArgumentosNoValida(int cantEspaciosCadena,int cantInvalidos);
int cantidadDeArgumentos(int cantEspaciosCadena,int cantValidos);
int cadenasIguales(char *cadena1,char *cadena2);
int noSeIngresoNada(char *cadena);
char *borrarEspaciosInnecesarios(char *cadena);
char* retornarBufferPedido(char **operacion,char *cadena,int tamSeg);
int consola(void);
void manejo_consola();

#endif /* FUNC_CONSOLA_H_ */
