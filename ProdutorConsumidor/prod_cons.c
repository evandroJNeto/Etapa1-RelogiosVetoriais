/*
 * Compilação: gcc -g -Wall -o prod_cons prod_cons_clocks.c -lpthread
 * Execução:   ./prod_cons
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

#define PRO_NUM 3 // quantidade de threads produtoras
#define CON_NUM 3 // quantidade de threads consumidoras
#define CAPAC_FILA 5 

/* Estrutura do relógio vetorial (3 processos). */
typedef struct Clock {
    int p[3];
    int id;
} Clock;

Clock clockQueue[CAPAC_FILA]; // Fila de relógios vetoriais
int clockCount = 0; // Contador de relógios na fila

pthread_mutex_t mutex; // Mutex para proteger o acesso à fila de relógios
pthread_cond_t condFull; // Condição para fila cheia
pthread_cond_t condEmpty; // Condição para fila vazia

Clock getClock() {
    pthread_mutex_lock(&mutex);
    
    while (clockCount == 0) {
        printf("A fila está vazia no atual momento\n");
        pthread_cond_wait(&condEmpty, &mutex);
    }
    
    Clock clock = clockQueue[0];
    for (int i = 0; i < clockCount - 1; i++) {
        clockQueue[i] = clockQueue[i + 1];
    }
    clockCount--;
    
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condFull);
    return clock;
}

int main(void) {
    int my_rank;
    int comm_sz;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0)
        process0();
    else if (my_rank == 1)
        process1();
    else if (my_rank == 2)
        process2();

    MPI_Finalize();
    return 0;
}