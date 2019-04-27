#include "stel.h"
#include "list.h"


int is_verbose = 0;
int is_random = 0;

static int typeOfCall() {
    return (((double)rand())/RAND_MAX) < PROB_ESPECIALIZADA ? CHEGADA_ESPECIALIZADA : CHEGADA_GERAL;
}

static double exponential(double media, double min, double max){
    double u_d = ((double)rand())/RAND_MAX;
    double d = (-(media)*log(u_d)); 
 
    return (d < max)? (d > min)? d : min : max;
}

// Gaussian distribution with low and high limits
// TOCAR PELO METODO 3 - Mais tarde
static double gaussian(void){
   static double v, fac;
   static int phase = 0;
   double S, Z, U1, U2, u;

    if (phase)
        Z = v * fac;
    else{
      do{
         U1 = (double)rand() / RAND_MAX;
         U2 = (double)rand() / RAND_MAX;

         u = 2. * U1 - 1.;
         v = 2. * U2 - 1.;
         S = u * u + v * v;
      } while (S >= 1);

      fac = sqrt (-2. * log(S) / S);
      Z = u * fac;
   }

   phase = 1 - phase;

   return ((GAUSSIAN_MEAN + GAUSSIAN_STD_DEV *Z) < GAUSSIAN_MAX)? ((GAUSSIAN_MEAN + GAUSSIAN_STD_DEV *Z) > GAUSSIAN_MIN)? (GAUSSIAN_MEAN + GAUSSIAN_STD_DEV *Z): GAUSSIAN_MIN : GAUSSIAN_MAX;
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

int parse_input3(int argc, char** argv, struct simulacao *simulacao_atual){
    extern char *optarg;
    extern int optind, opterr, optopt;
    int args_obrigatorios = 3, c;

    while ((c = getopt (argc, argv, "s:r:w:vch")) != -1)
    switch (c) {
        case 'h':
            printf("\n"UNDERLI"\tDiscrete Event Traffic Simulation - STEL 2018/2019 @ FEUP"END_UND);
            printf("\n\nUsage: %s\t {-s NrOfSamples} {-r NrOfResources} {-w WaitingListLength} [-v] [-c]\n"
                   "\n\t-v\tMakes the program verbose\n"
		   "\t-c\tSeed the PRNG with the current system time\n\n", argv[0]);
            return -1;
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
            if ( optopt == 's' || optopt == 'r' || optopt == 'w')
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


// Funcao que gera chamada, determinando se esta sera atendida por General Purpose
// ou Specific Purpose. Com base nisso, define o tipo de chamada, e a sua duracao.
double gerarChamada(struct simulacao *simulacao_atual, double ultima_chegada, double *delay){
    double u_c = ((double)rand())/RAND_MAX;
    double c = -(1.0/ (*simulacao_atual).taxa_chegada )*log(u_c);		// Intervalo para proxima chegada
    double tempo_recurso_libertado = 0;
    double tempo_atual = ultima_chegada+c;

    (*simulacao_atual).nr_processadas++;
    
    double partida_a_libertar = ( (*simulacao_atual).lista_recursos == NULL)?(tempo_atual+1):( ((lista*)(*simulacao_atual).lista_recursos)->tempo);
    while (partida_a_libertar < tempo_atual){
        // Existe uma partida nao processada - libertar recurso
        (*simulacao_atual).recursos_ocupados--;
        tempo_recurso_libertado = ((lista*)(*simulacao_atual).lista_recursos)->tempo;
        (*simulacao_atual).lista_recursos = remover((*simulacao_atual).lista_recursos);

        if ((*simulacao_atual).lista_espera_ocupada){
            // Existindo evento na lista de espera, processar esse
            (*delay) += tempo_recurso_libertado - ((lista*)(*simulacao_atual).lista_espera)->tempo;
            double tempo_inicial = ((lista*)(*simulacao_atual).lista_espera)->tempo;
            int tipo_chamada = ((lista*)(*simulacao_atual).lista_espera)->tipo;
            double this_delay = tempo_recurso_libertado - ((lista*)(*simulacao_atual).lista_espera)->tempo;
            (*simulacao_atual).lista_espera = remover((*simulacao_atual).lista_espera);
            

            //printf("delay: %f, position: %d\n", this_delay, (int)((this_delay)/0.01));
            if ((this_delay) >= (N_HIST-1)*(0.01)) (*simulacao_atual).hist_delay[N_HIST-1]++;
            else (*simulacao_atual).hist_delay[(int)((this_delay)/0.01)]++;
            // UMA CHAMADA EM LISTA DE ESPERA FOI ATENDIDA, TEMPO QUE ESPEROU: tempo_recurso_libertado - tempo_inicial
            
            // DURACAO DEPENDENTE DO TIPO DE CHAMADA (GP OU SP)
            if (tipo_chamada == CHEGADA_GERAL){
                double d = EXPONENT_FIX + exponential(EXPONENT_AVG,EXPONENT_MIN,EXPONENT_MAX);
                (*simulacao_atual).lista_recursos = adicionar((*simulacao_atual).lista_recursos, PARTIDA_GERAL, tempo_recurso_libertado+d);
            } else {
                double d = gaussian();
                (*simulacao_atual).lista_recursos = adicionar((*simulacao_atual).lista_recursos, PARTIDA_ESPECIALIZADA, tempo_recurso_libertado+d);
            }
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
            // CHAMADA VAI SER ATRASADA, MAS NAO BLOQUEADA
            (*simulacao_atual).lista_espera_ocupada++;
            (*simulacao_atual).lista_espera = adicionar((*simulacao_atual).lista_espera, CHEGADA, tempo_atual);
        } else {
            // CHAMADA BLOQUEADA
            (*simulacao_atual).nr_bloqueadas++;
        }
    } else {
        // Adicionar chegada, ocupar recurso, gerar partida
        (*simulacao_atual).recursos_ocupados++;

        // PARTIDA DEPENDE DO TIPO DE CHAMADA, SP OU GP
        if (typeOfCall() == CHEGADA_GERAL){
            double d = EXPONENT_FIX + exponential(EXPONENT_AVG,EXPONENT_MIN,EXPONENT_MAX);
            (*simulacao_atual).lista_recursos = adicionar((*simulacao_atual).lista_recursos, PARTIDA_GERAL, tempo_atual+d);
        } else {
            double d = gaussian();
            (*simulacao_atual).lista_recursos = adicionar((*simulacao_atual).lista_recursos, PARTIDA_ESPECIALIZADA, tempo_atual+d);
        }
    }
  
    return tempo_atual;
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

    pthread_exit(NULL);
}
