#ifndef SOCKETS_H_
#define SOCKETS_H_


char* sendMessage(int sockfd, char *msg);
char* recieveMessage(int sockfd);
int openSocketClient(int port, char *ip);
int openSocketServerPLP(int port);
int openSocketServerPCP(int port);

#endif /* SOCKETS_H_ */
