#include "stel.h"
#include "list.h"

static int is_verbose = 0;

int parse_input(int argc, char** argv, double * lambda, int * nr_amostras, double * delta, int * N_HIST, int * N_resources){

    if (argc < 5){
        printf("Usage: <-opt> LAMBDA NR_SAMPLES DELTA_HIST N_HIST N_RESOURCES\n  -v\t Make the program verbose\n");
        return -1;
    }
    if(argc == 6){
        if((argv[1][0] == '-') && (argv[1][1] == 'v')) is_verbose = 1; 
    }
    *lambda = atof(argv[1+is_verbose]);
    *nr_amostras = atoi(argv[2 + is_verbose]);
    *delta = atof(argv[3 + is_verbose]);
    if ((*delta) > (0.2/(*lambda))){
        printf("DELTA or LAMBDA too big.\n");
        return -1;
    }
    *N_HIST = atoi(argv[4 + is_verbose]);
    if ((*N_HIST-2)*(*delta) < 5*(1.0/(*lambda))){
        printf("N_HIST too small.\n");
        return -1;
    }
	*N_resources = atoi(argv[5 + is_verbose]);
    if (*N_resources < 0){
        printf("Resources cannot be negative.\n");
        return -1;
    }
    return 0;
}

int parse_input2(int argc, char** argv, double *lambda, double *dm, int *sample_nr, int *resource_nr, int *waiting_length){

    if (argc < 6){
        printf("Usage: <-opt> ArrivalRate AverageDuration NrOfSamples NrOfResources WaitingListLength\n  -v\t Make the program verbose\n");
        return -1;
    }
    if(argc == 7){
        if((argv[1][0] == '-') && (argv[1][1] == 'v')) is_verbose = 1; 
    }

    *lambda = atof(argv[1 + is_verbose]);
    *dm = atof(argv[2 + is_verbose]);
    *sample_nr = atoi(argv[3 + is_verbose]);
    *resource_nr = atoi(argv[4 + is_verbose]);
    *waiting_length = atoi(argv[5 + is_verbose]);
    
    /*
    if ((*delta) > (0.2/(*lambda))){
        printf("DELTA or LAMBDA too big.\n");
        return -1;
    }
    *N_HIST = atoi(argv[4]);
    if ((*N_HIST-2)*(*delta) < 5*(1.0/(*lambda))){
        printf("N_HIST too small.\n");
        return -1;
    }
    */
    return 0;
}
// &lista_eventos, &lista_partidas, &lista_espera, lambda, dm, ultima_chegada, &recursos_ocupados, &bloqueadas, n_recursos, tamanho_espera
double gerarEvento(lista **lista_ev, lista **lista_partidas, lista **lista_espera, double lambda, double dm, double ultima_chegada, 
                   int *recursos_ocupados, int *bloqueadas, int n_recursos, int tamanho_espera, int *espera_ocupada, int *nr_atrasadas, double *delay){
    double u_c = ((double)rand())/RAND_MAX;
    double c = -(1.0/lambda)*log(u_c);		// Intervalo para proxima chegada
    double tempo_recurso_libertado = 0;
    
    double partida_a_libertar = (*lista_partidas == NULL)?(ultima_chegada+c+1):((*lista_partidas)->tempo);
    if(is_verbose)printf("Partida a libertar: %lf\tUltima chegada: %lf\n", partida_a_libertar, ultima_chegada+c);

    while (partida_a_libertar < (ultima_chegada+c)){
        (*recursos_ocupados)--;
        tempo_recurso_libertado = (*lista_partidas)->tempo;
        *lista_partidas = remover(*lista_partidas);

        if (*espera_ocupada){
            (*delay) += tempo_recurso_libertado - (*lista_espera)->tempo;
            *lista_espera = remover(*lista_espera);
            *lista_ev = adicionar(*lista_ev, CHEGADA, tempo_recurso_libertado);
            
            double u_d = ((double)rand())/RAND_MAX;
            double d = -dm*log(u_d);      // Duracao da chamada
            *lista_ev = adicionar(*lista_ev, PARTIDA, tempo_recurso_libertado+d);
            *lista_partidas = adicionar(*lista_partidas, PARTIDA, tempo_recurso_libertado+d);

            (*espera_ocupada)--;
            (*recursos_ocupados)++;
        }

        if (*lista_partidas == NULL)
            break;
        partida_a_libertar = (*lista_partidas)->tempo;
    }


    /*
    // LIMPAR LISTA DE ESPERA, SE POSSIVEL (?)
    while ((*espera_ocupada) && (*recursos_ocupados < n_recursos)){
        // delay = ultima_chegada+c-((*lista_espera)->tempo);
        *lista_ev = adicionar(*lista_ev, CHEGADA, ultima_chegada+c);
        (*espera_ocupada)--;
        (*recursos_ocupados)++;
    }
    */

    if (*recursos_ocupados >= n_recursos){
        // Adicionar a lista de espera, ou bloquear
        (*nr_atrasadas)++;
        if ((*espera_ocupada) < tamanho_espera){
            (*espera_ocupada)++;
            *lista_espera = adicionar(*lista_espera, CHEGADA, ultima_chegada+c);
        } else {
            (*bloqueadas)++;
        }
        //*lista_ev = adicionar(*lista_ev, BLOQUEA, ultima_chegada+c);
    } else {
        // Adicionar chegada, ocupar recurso, gerar partida
        (*recursos_ocupados)++;
        *lista_ev = adicionar(*lista_ev, CHEGADA, ultima_chegada+c);

        double u_d = ((double)rand())/RAND_MAX;
        double d = -dm*log(u_d);      // Duracao da chamada
        *lista_ev = adicionar(*lista_ev, PARTIDA, ultima_chegada+c+d);
        *lista_partidas = adicionar(*lista_partidas, PARTIDA, ultima_chegada+c+d);
    }
    return ultima_chegada+c;
    // HISTOGRAMA
    // if (c >= (N_HIST-1)*delta) histograma[N_HIST-1]++;
    // else histograma[(int)(c/delta)]++;
    // FIM DE HISTOGRAMA
}

int print_hist(char * csv_file){
    char cmd[255];
    sprintf(cmd, "./hist.sh %s", csv_file);
    return system(cmd);
}
void print_csv(int * hist, int hist_size ,char * csv_file){
    FILE *file_out;
	file_out = fopen(csv_file, "w+");
	for (int j = 0; j < hist_size-1; j++)
		fprintf(file_out, "%d,%d\n", j, hist[j]);
	fclose(file_out);
}

unsigned int factorial(unsigned int n){
    unsigned int ret_val = 1;
    for (int i = n; i > 1; i--)
 		ret_val *= i;
 	return ret_val;
}

double P_T(double lambda, unsigned int K, int T){
    return pow(lambda*T, K)*exp(-lambda*T)/factorial(K);
}

double F_c(double lambda, double t) {
    return 1 - exp(-lambda*t);
}

double f_c(double lambda, double t) {
    return lambda*exp(-lambda*t);
}
