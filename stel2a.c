#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "list.h"
#include "stel.h"

int main(int argc, char** argv){
    lista *lista_eventos, *lista_partidas, *lista_espera;
    lista_eventos = NULL;
    lista_partidas = NULL;
    lista_espera = NULL;

    double lambda, dm; //,delta;
    // double tempo_ev;
    int nr_amostras = 0, n_recursos, tamanho_espera=0, espera_ocupada=0; //, N_HIST;
    int recursos_ocupados = 0, bloqueadas = 0;

    double ultima_chegada = 0;

    //if (parse_input(argc, argv, &lambda,&nr_amostras,&delta,&N_HIST,&n_recursos)) return -1;
    if(parse_input2(argc, argv, &lambda, &dm, &nr_amostras, &n_recursos, &tamanho_espera)) return -1;

    srand(1);
    //int *histograma = (int*) calloc(N_HIST, sizeof(int));

    for (int i = 0; i < nr_amostras; i++){
        ultima_chegada = gerarEvento(&lista_eventos, &lista_partidas, &lista_espera, lambda, dm, ultima_chegada, 
                                     &recursos_ocupados, &bloqueadas, n_recursos, tamanho_espera, &espera_ocupada);
        //if (c >= (N_HIST-1)*delta) histograma[N_HIST-1]++;
        //else histograma[(int)(c/delta)]++;
    }

    //printf("\nLISTA ACTUAL\n");
    //imprimir(lista_eventos);
    //printf("Lista partidas\n");
    //imprimir(lista_partidas);
    //printf("Intervalo medio entre chegada de eventos: %lf\n", ultima_chegada/N_HIST);
    printf("Numero de bloqueadas: %d\n", bloqueadas);
    printf("Probabilidade de bloqueio: %f\n\n", ((double)bloqueadas/nr_amostras)*100.0);
    /*
	printf("Escrever ficheiro CSV? [y/N]  ");
	char ans = getchar();
	if (ans == 'y' || ans == 'Y'){
		print_csv(histograma,sizeof(histograma)/sizeof(int),"./output.csv");		
		ans = getchar();
		printf("Apresentar histograma? [y/N]  ");
		ans = getchar();
		if (ans == 'y' || ans == 'Y')
			print_hist("./output.csv");
	}
    */

    return 0;
}
