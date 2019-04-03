#include <stdio.h>
#include <stdlib.h>
#ifndef LIST_H
#define LIST_H

typedef struct lista{
	int tipo;
	double tempo;
	struct lista * proximo;
} lista;

lista * remover (lista * apontador);
lista * adicionar (lista * apontador, int n_tipo, double n_tempo);
void imprimir (lista * apontador);

#endif