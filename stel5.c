#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "list.h"
#include "stel.h"

/*
Procedimento:
Ao gerar chamada:
 - Calcular a hora a que esta chegou (tempo atual da simulação)
 - Se existirem chamadas anteriores que já terminaram, libertar esses recurso
    (chamadas/recursos de GP ou SP, tanto faz)
 - Se existirem chamadas em lista de espera, atender primeiro, ocupando recurso
 - Ao atender, definir a duração da chamada, logo, a hora do seu fim no sistema GP
 - Se for chamada SP, gerar o evento SP (com hora de início no fim da chamada GP)
 - Depois de atender todas na lista de espera, se existirem recursos livres, a atual é
    atendida (e gera-se duração e fim). Se não existirem recursos disponíveis, lista de
    espera (se tiver lugares livres). Se lista de espera ocupada, bloquear.

Ao gerar chamada SP:
(semelhante ao anterior)
 - Ja se sabe a hora a que ele chegou, nao é preciso calcular
 - Se existirem chamadas SP anteriores que já terminaram, libertar recursos
 - Se existirem chamadas SP em lista de espera, atender primeiro
 - Ao atender, definir duração da chamada e hora do fim
 - Se possivel, atender esta (e gerar fim), se não, lista de espera (infinita)

Estruturas necessárias:
 - lista de espera para chamadas GP (tamanho finito)
 - lista de libertação de recursos GP (tamanho = nr de recursos)
 - lista de eventos (uma para GP e outra SP ???) se calhar é mais util termos duas, mas não estou a ver (honestamente dava para fazer com zero, acho)
 - lista de espera para chamadas SP (infinita)
 - lista de libertação de recursos SP
 
Funções necessárias:

Estatística necessária:
- Probabilida de uma chamada ser colocada em espera e ser perdida
- Tempo de espera média e função de distribuição do tempo de espera 
- Histograma da previsão do tempo de espera e a média absoluta
 e relativa da previsão do tempo de espera

*/

int main(int argc, char** argv){
    struct simulacao simulacao_atual;
    pthread_t printProg_thr;
    
    // Definir parametros do sistema
    simulacao_atual = (struct simulacao) {
        .lista_espera = NULL, .lista_recursos = NULL, .nr_amostras = 10000, .nr_processadas = 0,
        .tamanho_espera = 10, .recursos_ocupados = 0, .lista_espera_ocupada = 0, .nr_bloqueadas = 0,
        .nr_atrasadas = 0, .taxa_chegada = 80.0/60, .duracao_media= 2, .nr_recursos = 5
    };

    double ultima_chegada = 0, delay = 0;

    is_random = 0;
    is_verbose = 0;
    
    if(parse_input3(argc, argv, &simulacao_atual)){
        return -1;
    }

    if(!is_random)srand(1); // Use always the same seed for debugging purposes
    else srand(time(NULL));


    pthread_create(&printProg_thr, NULL, print_prog, &simulacao_atual);
    
    for (int i = 0; i < simulacao_atual.nr_amostras; i++){
        ultima_chegada = gerarChamada(&simulacao_atual, ultima_chegada, &delay);
    }

    pthread_join(printProg_thr, NULL);
    
    //printf("Intervalo medio entre chegada de eventos: %lf\n", ultima_chegada/N_HIST);
    printf("Numero de bloqueadas: %d\n", simulacao_atual.nr_bloqueadas);
    printf("Probabilidade de bloqueio: %f\n", ((double)simulacao_atual.nr_bloqueadas/simulacao_atual.nr_amostras)*100.0);
    printf("Numero de atrasadas: %d\n", simulacao_atual.nr_atrasadas);
    printf("Probabilidade de atraso: %f\n", ((double)simulacao_atual.nr_atrasadas/simulacao_atual.nr_amostras)*100.0);
    printf("Atraso medio: %f\n\n", ((double)delay/simulacao_atual.nr_amostras));// o delay esta em que unidade?

	printf("Escrever ficheiro CSV? [y/N]  ");
	char ans = getchar();
	if (ans == 'y' || ans == 'Y'){
		print_csv(simulacao_atual.hist_delay,sizeof(simulacao_atual.hist_delay)/sizeof(int),"./output.csv");		
		ans = getchar();
		printf("Apresentar histograma? [y/N]  ");
		ans = getchar();
		if (ans == 'y' || ans == 'Y')
			print_hist("./output.csv");
	}
    
    return 0;
}
