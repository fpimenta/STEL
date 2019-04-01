#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"

#define CHEGADA 0
#define PARTIDA 1
#define BLOQUEA 2

unsigned int factorial(unsigned int n);
double P_T(double lambda, unsigned int K, int T);
double F_c(double lambda, double t);
double f_c(double lambda, double t);

int print_hist(char * csv_file);
void print_csv(int * hist, int hist_size ,char * csv_file);
int parse_input(int argc, char** argv, double * lambda, int * nr_amostras, double * delta, int * N_HIST, int * N_resources);
int parse_input2(int argc, char** argv, double *lambda, double *dm, int *sample_nr, int *resource_nr, int *waiting_length);
double gerarEvento(lista **lista_ev, lista **lista_partidas, lista **lista_espera, double lambda, double lm, double ultima_chegada, 
                   int *recursos_ocupados, int *bloqueadas, int n_recursos, int tamanho_espera, int *espera_ocupada, int *nr_atrasadas, double *delay);

