#include <stdio.h>
#include <stdlib.h>
#ifndef LIST_H
#define LIST_H

typedef struct{
	int tipo;
	double tempo;
	struct lista * proximo;
} lista;

lista * remover (lista * apontador);
lista * adicionar (lista * apontador, int n_tipo, double n_tempo);
void imprimir (lista * apontador);

/*
int main(void)
{
	if ( lista_eventos != NULL)
	{
		tipo_ev = lista_eventos -> tipo;
		tempo_ev = lista_eventos -> tempo;
		lista_eventos = remover(lista_eventos);
		printf("\nEVENTO REMOVIDO\n");
		printf("Tipo=%d\tTempo=%lf\n", tipo_ev, tempo_ev);
	}
	printf("\nLISTA ACTUAL\n");
	imprimir(lista_eventos);
}
*/
#endif