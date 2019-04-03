#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "list.h"
#include "stel.h"

int main(int argc, char** argv){
    struct simulacao simulacao_atual;
    simulacao_atual = (struct simulacao) {
        .lista_eventos = NULL, .lista_espera = NULL, .lista_recursos = NULL,
        .nr_amostras = 0, .tamanho_espera = 0, .recursos_ocupados = 0,
        .lista_espera_ocupada = 0, .nr_bloqueadas = 0, .nr_atrasadas = 0
    };

    double ultima_chegada = 0, delay = 0;

    if(parse_input2(argc, argv, &simulacao_atual)){
        return -1;
    }
    srand(1);

    for (int i = 0; i < simulacao_atual.nr_amostras; i++)
        ultima_chegada = gerarEvento2(&simulacao_atual, ultima_chegada, &delay);
    

    //printf("\nLISTA ACTUAL\n");
    //imprimir(lista_eventos);
    //printf("Lista partidas\n");
    //imprimir(lista_partidas);
    //printf("Intervalo medio entre chegada de eventos: %lf\n", ultima_chegada/N_HIST);
    printf("Numero de bloqueadas: %d\n", simulacao_atual.nr_bloqueadas);
    printf("Probabilidade de bloqueio: %f\n", ((double)simulacao_atual.nr_bloqueadas/simulacao_atual.nr_amostras)*100.0);
    printf("Numero de atrasadas: %d\n", simulacao_atual.nr_atrasadas);
    printf("Probabilidade de atraso: %f\n", ((double)simulacao_atual.nr_atrasadas/simulacao_atual.nr_amostras)*100.0);
    printf("Atraso medio: %f\n\n", ((double)delay/simulacao_atual.nr_amostras));
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
