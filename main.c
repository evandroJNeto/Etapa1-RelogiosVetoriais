/*
 * Mizael Paris Leite
 * Evandro José dos Santos Neto
 * Compilação: mpicc -o main main.c
 * Execução:   mpiexec -n 3 --oversubscribe ./main
*/

#include <stdio.h>
#include <string.h>
#include <mpi.h>

/* Estrutura do relógio vetorial (3 processos). */
typedef struct Clock {
    int p[3];
} Clock;

/*
 * Imprime o estado atual do relógio vetorial.
 * Mostra: qual processo realizou a atualização e o vetor resultante.
*/
void PrintClock(const char *acao, Clock *clock) {
    int pid;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    printf("[Atualizado por P%d] %s -> Clock = (%d,%d,%d)\n",
           pid, acao, clock->p[0], clock->p[1], clock->p[2]);
}

/*
 * Evento interno (sem comunicação).
 * Incrementa o contador local do processo.
*/
void Event(Clock *clock) {
    int pid;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid); // Obtem o rank e salva na variavel pid
    clock->p[pid]++; // Incrementa o clock local
    
    // No final, imprime relógio atualizado
    PrintClock("Evento interno", clock);
}

void Send(int dest, Clock *clock) {
    int pid;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid); // Obtem o rank e salva na variavel pid
    clock->p[pid]++; // Incrementa o clock local
    MPI_Send(clock->p, 3, MPI_INT, dest, 0, MPI_COMM_WORLD); // Envia o clock local via MPI_Send

    // No final, imprime relógio atualizado:
    PrintClock("Envio de mensagem", clock);
}

void Receive(int src, Clock *clock) {
    int pid;
    int recebe[3]; // Vetor que vai receber a informação do clock de outro processo
    MPI_Comm_rank(MPI_COMM_WORLD, &pid); // Obtem o rank e salva na variavel pid
    clock->p[pid]++; // Incrementa o clock local
    MPI_Recv(recebe, 3, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recebe as informações do clock de outro processo
    
    for (int i= 0; i < 3; i++) { // Laço para comparar os valores e substituir pelo maior
        if (clock->p[i] < recebe[i]) {
            clock->p[i] = recebe[i];
        }
    }

    // No final, imprime relógio atualizado
    PrintClock("Recebimento de mensagem", clock);
}

/*
 * Cada processo define sua sequência de eventos e comunicações
 * conforme o diagrama da figura de referência.
*/

void process0() {
    Clock clock = {{0,0,0}};
    PrintClock("Estado inicial", &clock);

    Event(&clock);
}

void process1() {
    Clock clock = {{0,0,0}};
    PrintClock("Estado inicial", &clock);

}

void process2() {
    Clock clock = {{0,0,0}};
    PrintClock("Estado inicial", &clock);

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