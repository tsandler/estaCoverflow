/*
 * kernelSocket.h
 *
 *  Created on: 18/06/2014
 *      Author: utnso
 */

#ifndef KERNELSOCKET_H_
#define KERNELSOCKET_H_

#include "globales.h"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <commons/collections/queue.h>
#include <libs/manejoDelPCB.h>
#include <libs/sockets.h>
#include <pthread.h>
#include "manejoCPU.h"
#include "colas.h"


int openSocketServerPLP(int port);
int openSocketServerPCP(int port);


#endif /* KERNELSOCKET_H_ */
