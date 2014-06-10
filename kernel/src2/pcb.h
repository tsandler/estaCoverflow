#ifndef PCB_H_
#define PCB_H_

typedef struct {

	int PID;
	int* segmentCode;
	int* segmentStack;
	int* cursorStack;
	int* indiceCodigo;
	int* indiceEtiqueta;
	int programCounter;
	int sizeOfContext;
	int sizeOfEtiquetas;
	int peso;

}t_pcb ;

t_pcb* armarPCB(char* program);

#endif /* PCB_H_ */
