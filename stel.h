#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <sched.h>
#include "list.h"
#ifndef STEL_H
#define STEL_H

#define CHEGADA 0
#define PARTIDA 1
#define BLOQUEA 2
#define UNDERLI "\033[4m"
#define END_UND "\033[0m"

extern int is_verbose;
extern int is_random;

struct simulacao {
	struct lista * lista_eventos;
    struct lista * lista_espera;
    struct lista * lista_recursos;
    double taxa_chegada;
    double duracao_media;
    int nr_amostras;
    int nr_processadas;
    int nr_recursos;
    int tamanho_espera;
    int recursos_ocupados;
    int lista_espera_ocupada;
    int nr_bloqueadas;
    int nr_atrasadas;
};

int print_hist(char * csv_file);
void *print_prog( void * data_ptr);
void print_csv(int * hist, int hist_size ,char * csv_file);
int parse_input(int argc, char** argv, double *lambda, double *dm, int *sample_nr, int *resource_nr, int *waiting_length);
int parse_input2(int argc, char** argv, struct simulacao *simulacao_atual);
double gerarEvento(lista **lista_ev, lista **lista_partidas, lista **lista_espera, double lambda, double lm, double ultima_chegada, 
                   int *recursos_ocupados, int *bloqueadas, int n_recursos, int tamanho_espera, int *espera_ocupada, int *nr_atrasadas, double *delay);
double gerarEvento2(struct simulacao *simulacao_atual, double ultima_chegada, double *delay);
unsigned int factorial(unsigned int n);
double P_T(double lambda, unsigned int K, int T);
double F_c(double lambda, double t);
double f_c(double lambda, double t);

#endif
