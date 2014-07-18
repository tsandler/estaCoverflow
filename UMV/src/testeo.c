/*
 * testeo.c
 *
 *  Created on: 26/05/2014
 *      Author: utnso
 */

#include "testeo.h"
#include "functions.h"


void imprime_listatablaSegUMV(t_list* lista){
	list_iterate(lista,(void*)imprime_campos_listatablaSegUMV);
}

void test_crear_agregar_segmento(int pid, int tamanio){
	int dirLogica;

//	printf("Ingresar pid: ");
//	scanf("%d",&pid);
//	printf("Ingresar tamanio segmento: ");
//	scanf("%d",&tamanio);

	dirLogica = crear_agregar_segmento(pid,tamanio);

	log_info(logs,"Imprime contenido del segmento (desde diccionario)");
	t_list* listaSeg = dictionary_get(tablaPidSeg,string_itoa(pid));
	imprime_listatablaSegUMV(listaSeg);
	printf("\nDireccion logica: %d\n",dirLogica);
}

void test_crea_elimina_UMV(){

	inicializar_umv(tamanioUMV);
	imprime_listahuecos(listaHuecos);
	eliminarUMV();
}

void test_first_fit(){
	int tamanio;
	printf("Ingresar tamanio -para buscar un hueco-:");
	scanf("%d",&tamanio);
	first_fit(tamanio);
}

void test_crear_varios_segmentos_y_eliminarlos(){

	int pid, i;
	int vec[2];

	for(i=0;i<3;i++){
		printf("\n\n\n\nIngresar segmento nro: %d\n",i);
		printf("Ingresar pid: ");
		scanf("%d",&pid);
		vec[i]=pid;
//		test_crear_agregar_segmento(pid);
	}
	log_debug(logs,"Comienza a destruir segmentos");
	for(i=0;i<3;i++){
		destruir_segmentos(vec[i]);
		log_debug(logs,"se elimino el segmento");
		imprime_listahuecos(listaHuecos);
	}

}

void test_crear_varios_segs_y_eliminar_uno_para_compactar(){

	log_info(logs,"crea");
	test_crear_agregar_segmento(1,10);
	test_crear_agregar_segmento(2,20);

	log_info(logs,"destruye");
	destruir_segmentos(1);

	imprime_listahuecos(listaHuecos);

	log_info(logs,"compacta");
	compactar_memoria();

	log_info(logs,"crea denuevo");
	test_crear_agregar_segmento(2,10);

	imprime_listahuecos(listaHuecos);
}
