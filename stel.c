#include "stel.h"
#include "list.h"

int is_verbose = 0;
int is_random = 0;

int espec = 0; int ger = 0;

static int typeOfCall() {
    

    if ((((double)rand())/(double)RAND_MAX) < PROB_ESPECIALIZADA){
        espec++;
        //printf("Espec: %d Geral: %d\n", espec, ger);
        return CHEGADA_ESPECIALIZADA;
    } else {
        ger++;
        //printf("Espec: %d Geral: %d\n", espec, ger);
        return CHEGADA_GERAL;
    }
    //return ((((double)rand())/(double)RAND_MAX) < PROB_ESPECIALIZADA) ? CHEGADA_ESPECIALIZADA : CHEGADA_GERAL;
}

static double exponential(double media, double min, double max){
    double u_d = ((double)rand())/RAND_MAX;
    double d = (-(media)*log(u_d)) + min; 
    
    if( d < max)return d;
    else return max;
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
    if((GAUSSIAN_MEAN + GAUSSIAN_STD_DEV *Z) < 2){
    if((GAUSSIAN_MEAN + GAUSSIAN_STD_DEV *Z) > 0.5){
            return (GAUSSIAN_MEAN + GAUSSIAN_STD_DEV *Z);
    }else return 0.5;
   

   } return 2;
   //return ((GAUSSIAN_MEAN + GAUSSIAN_STD_DEV *Z) < GAUSSIAN_MAX)? ((GAUSSIAN_MEAN + GAUSSIAN_STD_DEV *Z) > GAUSSIAN_MIN)? (GAUSSIAN_MEAN + GAUSSIAN_STD_DEV *Z): GAUSSIAN_MIN : GAUSSIAN_MAX;
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
            if (optopt == 's' || optopt == 'r' || optopt == 'w')
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
double gerarChamada(struct simulacao *simulacao_atual, double ultima_chegada, double *delay, double * hist_delay, double * hist_previsao){
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
            double tempo_inicial = ((lista*)(*simulacao_atual).lista_espera)->tempo;
            double this_delay = tempo_recurso_libertado - tempo_inicial;
            (*delay) += this_delay;

            // Para auxiliar no histograma
            if(this_delay > (*simulacao_atual).max_delay) (*simulacao_atual).max_delay = this_delay;
            if(this_delay < (*simulacao_atual).min_delay) (*simulacao_atual).min_delay = this_delay;
            
            int tipo_chamada = typeOfCall();//((lista*)(*simulacao_atual).lista_espera)->tipo;
            //printf("nr_lista: %d espera_estimada: %f erro previsao: %f this_delay: %f\n",(*simulacao_atual).lista_espera_ocupada,((lista*)(*simulacao_atual).lista_espera)->espera_estimada ,((lista*)(*simulacao_atual).lista_espera)->espera_estimada - this_delay, this_delay);
            
            hist_previsao[(*simulacao_atual).nr_processadas] = ((lista*)(*simulacao_atual).lista_espera)->espera_estimada - this_delay;
            if((((lista*)(*simulacao_atual).lista_espera)->espera_estimada - this_delay) < (*simulacao_atual).pior_previsao)(*simulacao_atual).pior_previsao = ((lista*)(*simulacao_atual).lista_espera)->espera_estimada - this_delay; 
            (*simulacao_atual).lista_espera = remover((*simulacao_atual).lista_espera);
            //static int hist_pointer = 0;
            hist_delay[ (*simulacao_atual).nr_processadas] = this_delay;
            
            //if ((this_delay) >= (N_HIST-1)*(D_HIST)) hist_delay[N_HIST-1]++;
            //else hist_delay[(int)((this_delay)/D_HIST)]++;
            
            // UMA CHAMADA EM LISTA DE ESPERA FOI ATENDIDA, TEMPO QUE ESPEROU: tempo_recurso_libertado - tempo_inicial
            // USAR ISTO PARA CALCULAR NOVO ESTIMATED_DELAY
            // DURACAO DEPENDENTE DO TIPO DE CHAMADA (GP OU SP)
            if (tipo_chamada == CHEGADA_GERAL){
                double d = exponential(EXPONENT_AVG,EXPONENT_MIN,EXPONENT_MAX);
                (*simulacao_atual).lista_recursos = adicionar((*simulacao_atual).lista_recursos, PARTIDA_GERAL, tempo_recurso_libertado+d,tempo_recurso_libertado);
                average(d);
            } else {
                double d = gaussian();
                (*simulacao_atual).lista_recursos = adicionar((*simulacao_atual).lista_recursos, PARTIDA_ESPECIALIZADA, tempo_recurso_libertado+d,tempo_recurso_libertado);
                average(d);
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
        if ((*simulacao_atual).lista_espera_ocupada < (*simulacao_atual).tamanho_espera){
            (*simulacao_atual).nr_atrasadas++;
            (*simulacao_atual).lista_espera_ocupada++;

            lista * elemento_a_esperar = ((lista*)(*simulacao_atual).lista_recursos);
            for (int i = 0; i < ((*simulacao_atual).lista_espera_ocupada -1)%(*simulacao_atual).nr_recursos; i++)
                elemento_a_esperar = elemento_a_esperar->proximo;
            double estimador = ((elemento_a_esperar->espera_estimada+average(-1))>tempo_atual)?(elemento_a_esperar->espera_estimada+average(-1)-tempo_atual):0;
            estimador += average(-1)*( ((*simulacao_atual).lista_espera_ocupada - 1)/((*simulacao_atual).nr_recursos) );
            
            (*simulacao_atual).lista_espera = adicionar((*simulacao_atual).lista_espera, CHEGADA, tempo_atual, estimador);
        } else {
            (*simulacao_atual).nr_bloqueadas++;
        }
    } else {
        // Adicionar chegada, ocupar recurso, gerar partida100
        (*simulacao_atual).recursos_ocupados++;

        // PARTIDA DEPENDE DO TIPO DE CHAMADA, SP OU GP
        if (typeOfCall() == CHEGADA_GERAL){
            double d = exponential(EXPONENT_AVG,EXPONENT_MIN,EXPONENT_MAX);
            (*simulacao_atual).lista_recursos = adicionar((*simulacao_atual).lista_recursos, PARTIDA_GERAL, tempo_atual+d,tempo_atual);
            average(d);
        } else {
            double d = gaussian();
           
            (*simulacao_atual).lista_recursos = adicionar((*simulacao_atual).lista_recursos, PARTIDA_ESPECIALIZADA, tempo_atual+d,tempo_atual);
            average(d);
        }
    }
    
    return tempo_atual;
}


int print_hist(char * csv_file){
    char cmd[255];
    sprintf(cmd, "./hist.sh %s", csv_file);
    return system(cmd);
}
void print_csv(int * hist, int hist_size ,char * csv_file, double inicio){
    FILE *file_out;
	file_out = fopen(csv_file, "w+");
    if(file_out == NULL){
        return;
    }
	for (int j = 0; j < hist_size; j++){
		fprintf(file_out, "%f,%d\n", j+inicio, hist[j]);
    }
	fclose(file_out);
}

void print_csv2(double * hist, int hist_size ,char * csv_file){
    FILE *file_out;
    file_out = fopen(csv_file, "w+");
    for (int j = 0; j < hist_size; j++)
        fprintf(file_out, "%d,%f\n", j, hist[j]);
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

// Calcular Running Average de duracao, tendo em conta o novo valor introduzido. 
// Fornecendo -1, apenas mostra media atual, sem atualizar valor.
double average(double valorAtual){
    static int nr_samples = 0;
    static double media_atual = 0;

    if(valorAtual != -1){
        media_atual = media_atual*nr_samples + valorAtual;
        nr_samples++;
        media_atual = media_atual/nr_samples;
    }
    //printf("Current average: %f \tCurrent nr of samples: %d \tValor Atual: %f\n", media_atual, nr_samples, valorAtual);
    return media_atual;
}

// Calculates the average and pushes the values further down the vector
//double windowAverage(double *callDuration, double lastDelay,int windowSize){
//    static double avg = 0;
//    static int i = 0;
//    avg+=lastDelay;
//    i++;
//    callDuration[0] = (double)avg/i;
    /*for(int i= 0; i < windowSize-1;i++){
        callDuration[i+1] = callDuration[i];
        if(i == 0){
            callDuration[0] = lastDelay;
        }
        
        avg+= callDuration[i];
    }*/
//    return (double)(avg/i);
//}