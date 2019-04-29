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

#define CHEGADA_GERAL 0
#define PARTIDA_GERAL 1
#define CHEGADA_ESPECIALIZADA 2
#define PARTIDA_ESPECIALIZADA 3
#define BLOQUEA_GERAL 4

#define PROB_ESPECIALIZADA 0.7

#define GAUSSIAN_MEAN 1 // gaussian centered at 1s
#define GAUSSIAN_STD_DEV (1.0/3) // with 20 seconds standard deviation
#define GAUSSIAN_MAX 2 // 120 seconds
#define GAUSSIAN_MIN 0.5 // 30 seconds

#define EXPONENT_AVG 2
#define EXPONENT_MIN 1
#define EXPONENT_MAX 5

#define CHEGADA 0
#define PARTIDA 1
#define BLOQUEA 2
#define UNDERLI "\033[4m"
#define END_UND "\033[0m"

#define N_HIST 100
#define D_HIST 0.1

#define MEAN_WINDOW 40

extern int is_verbose;
extern int is_random;

extern int espec; 
extern int ger;

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
    double duracao_media_real;
    double min_delay;
    double max_delay;
    double pior_previsao;
    double call_duration[MEAN_WINDOW];
};

int print_hist(char * csv_file);
void *print_prog( void * data_ptr);
void print_csv(int * hist, int hist_size ,char * csv_file, double inicio);
int parse_input2(int argc, char** argv, struct simulacao *simulacao_atual);
int parse_input3(int argc, char** argv, struct simulacao *simulacao_atual);
double gerarEvento2(struct simulacao *simulacao_atual, double ultima_chegada, double *delay);
double gerarChamada(struct simulacao *simulacao_atual, double ultima_chegada, double *delay, double* hist_delay, double *hist_previsao);
unsigned int factorial(unsigned int n);
//double windowAverage(double *callDuration, double lastDelay,int windowSize);
double average(double valorAtual);
//double average(double *callDuration, int windowSize);
void print_csv2(double * hist, int hist_size ,char * csv_file);

#endif
