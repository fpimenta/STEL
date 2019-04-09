#include "stel.h"
#include "list.h"


int is_verbose = 0;
int is_random = 0;

int parse_input(int argc, char** argv, double *lambda, double *dm, int *sample_nr, int *resource_nr, int *waiting_length){
    extern char *optarg;
    extern int optind, opterr, optopt;
    int args_obrigatorios = 5, c;

    while ((c = getopt (argc, argv, "a:d:s:r:w:vch")) != -1)
    switch (c) {
        case 'h':
            printf("\n"UNDERLI"\tDiscrete Event Traffic Simulation - STEL 2018/2019 @ FEUP"END_UND);
            printf("\n\nUsage: %s\t{-a ArrivalRate} {-d AverageDuration} {-s NrOfSamples}\n"
                   "\t\t{-r NrOfResources} {-w WaitingListLength} [-v] [-c]\n"
                   "\n\t-v\tMakes the program verbose\n\n"
		   "\n\t-c\tSeed the PRNG with the current system time\n\n", argv[0]);
            return -1;
        case 'a':
            args_obrigatorios--;
            *lambda = atof(optarg);
            break;
        case 'd':
            args_obrigatorios--;
            *dm = atof(optarg);
            break;
        case 's':
            args_obrigatorios--;
            *sample_nr = atoi(optarg);
            break;
        case 'r':
            args_obrigatorios--;
            *resource_nr = atoi(optarg);
            break;
        case 'w':
            args_obrigatorios--;
            *waiting_length = atoi(optarg);
            break;
        case 'v':
            is_verbose = 1;
            break;
	    case 'c':
	        is_random = 1;
	        break;
        case '?':
            if (optopt == 'a' || optopt == 'd' || optopt == 's' || optopt == 'r' || optopt == 'w')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'. Use -h for help.\n", optopt);
            else
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
        default:
            abort ();
        }
    if (args_obrigatorios > 0){
        printf("Not all values were specified. Use -h for help. Quitting...\n");
        return -1;
    }

    return 0;
}

int parse_input2(int argc, char** argv, struct simulacao *simulacao_atual){
    extern char *optarg;
    extern int optind, opterr, optopt;
    int args_obrigatorios = 5, c;

    while ((c = getopt (argc, argv, "a:d:s:r:w:vch")) != -1)
    switch (c) {
        case 'h':
            printf("\n"UNDERLI"\tDiscrete Event Traffic Simulation - STEL 2018/2019 @ FEUP"END_UND);
            printf("\n\nUsage: %s\t{-a ArrivalRate} {-d AverageDuration} {-s NrOfSamples}\n"
                   "\t\t{-r NrOfResources} {-w WaitingListLength} [-v] [-c]\n"
                   "\n\t-v\tMakes the program verbose\n"
		   "\t-c\tSeed the PRNG with the current system time\n\n", argv[0]);
            return -1;
        case 'a':
            args_obrigatorios--;
            (*simulacao_atual).taxa_chegada = atof(optarg);
            break;
        case 'd':
            args_obrigatorios--;
            (*simulacao_atual).duracao_media = atof(optarg);
            break;
        case 's':
            args_obrigatorios--;
            (*simulacao_atual).nr_amostras = atoi(optarg);
            break;
        case 'r':
            args_obrigatorios--;
            (*simulacao_atual).nr_recursos = atoi(optarg);
            break;
        case 'w':
            args_obrigatorios--;
            (*simulacao_atual).tamanho_espera = atoi(optarg);
            break;
        case 'v':
            is_verbose = 1;
            break;
	    case 'c':
	        is_random = 1;
	        break;
        case '?':
            if (optopt == 'a' || optopt == 'd' || optopt == 's' || optopt == 'r' || optopt == 'w')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'. Use -h for help.\n", optopt);
            else
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
        default:
            abort ();
        }
    if (args_obrigatorios > 0){
        printf("Not all values were specified. Use -h for help. Quitting...\n");
        return -1;
    }

    return 0;
}

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


double gerarEvento2(struct simulacao *simulacao_atual, double ultima_chegada, double *delay){
    double u_c = ((double)rand())/RAND_MAX;
    double c = -(1.0/ (*simulacao_atual).taxa_chegada )*log(u_c);		// Intervalo para proxima chegada
    double tempo_recurso_libertado = 0;

    (*simulacao_atual).nr_processadas++;
    
    double partida_a_libertar = ( (*simulacao_atual).lista_recursos == NULL)?(ultima_chegada+c+1):( ((lista*)(*simulacao_atual).lista_recursos)->tempo);
    if(is_verbose)printf("Partida a libertar: %lf\tUltima chegada: %lf\n", partida_a_libertar, ultima_chegada+c);

    while (partida_a_libertar < (ultima_chegada+c)){
        (*simulacao_atual).recursos_ocupados--;
        tempo_recurso_libertado = ((lista*)(*simulacao_atual).lista_recursos)->tempo;
        (*simulacao_atual).lista_recursos = remover((*simulacao_atual).lista_recursos);

        if ((*simulacao_atual).lista_espera_ocupada){
            (*delay) += tempo_recurso_libertado - ((lista*)(*simulacao_atual).lista_espera)->tempo;
            (*simulacao_atual).lista_espera = remover((*simulacao_atual).lista_espera);
            (*simulacao_atual).lista_eventos = adicionar((*simulacao_atual).lista_eventos, CHEGADA, tempo_recurso_libertado);
            
            double u_d = ((double)rand())/RAND_MAX;
            double d = -(*simulacao_atual).duracao_media*log(u_d);      // Duracao da chamada
            (*simulacao_atual).lista_eventos = adicionar((*simulacao_atual).lista_eventos, PARTIDA, tempo_recurso_libertado+d);
            (*simulacao_atual).lista_recursos = adicionar((*simulacao_atual).lista_recursos, PARTIDA, tempo_recurso_libertado+d);

            (*simulacao_atual).lista_espera_ocupada--;
            (*simulacao_atual).recursos_ocupados++;
        }

        if ((*simulacao_atual).lista_recursos == NULL)
            break;
        partida_a_libertar = ((lista*)(*simulacao_atual).lista_recursos)->tempo;
    }

    if ((*simulacao_atual).recursos_ocupados >= (*simulacao_atual).nr_recursos){
        // Adicionar a lista de espera, ou bloquear
        (*simulacao_atual).nr_atrasadas++;
        if ((*simulacao_atual).lista_espera_ocupada < (*simulacao_atual).tamanho_espera){
            (*simulacao_atual).lista_espera_ocupada++;
            (*simulacao_atual).lista_espera = adicionar((*simulacao_atual).lista_espera, CHEGADA, ultima_chegada+c);
        } else {
            (*simulacao_atual).nr_bloqueadas++;
        }
        //(*simulacao_atual).lista_eventos = adicionar((*simulacao_atual).lista_eventos, BLOQUEA, ultima_chegada+c);
    } else {
        // Adicionar chegada, ocupar recurso, gerar partida
        (*simulacao_atual).recursos_ocupados++;
        (*simulacao_atual).lista_eventos = adicionar((*simulacao_atual).lista_eventos, CHEGADA, ultima_chegada+c);

        double u_d = ((double)rand())/RAND_MAX;
        double d = -(*simulacao_atual).duracao_media*log(u_d);      // Duracao da chamada
        (*simulacao_atual).lista_eventos = adicionar((*simulacao_atual).lista_eventos, PARTIDA, ultima_chegada+c+d);
        (*simulacao_atual).lista_recursos = adicionar((*simulacao_atual).lista_recursos, PARTIDA, ultima_chegada+c+d);
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

void *print_prog( void * data_ptr){

    struct simulacao * simulacao_atual = (struct simulacao*)data_ptr;

    static struct timeval t1, t2;
    static double elapsedTime;

    while(1){
    
        double percentage = 100.0 * (*simulacao_atual).nr_processadas / (*simulacao_atual).nr_amostras;


        int pos;

        if(percentage <= 1){
            gettimeofday(&t1, NULL);
        }
        printf("\r %5.1f%%[", percentage);

        pos = percentage * 30 / 100.0;

        for (int i = 0; i < 30; i++){
            if( i <= pos)printf("=");
            else printf(".");
        }printf("]");
        
        fflush(stdout);
        if((*simulacao_atual).nr_processadas == (*simulacao_atual).nr_amostras)break;
        sched_yield();
    
    }

    gettimeofday(&t2, NULL);
    elapsedTime = (double)(t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (double)(t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    printf(" in %.3lf s \n\n",elapsedTime/1000.0);

    return NULL;

   
}
