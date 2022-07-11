#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include <time.h>
#define MASTER 0

double start = 0, end = 0;
int soma_total = 0;
int som_parciais = 0;
int trocaMax(int a, int b);
int trocaMim(int a, int b);

int main( int argc, char* argv[] )
{
   
    if( argc < 2 )
    {
        printf("Erro! Tente novamente e informe o tamanho do vetor.\n");
        exit(0);
    }

    int rank, wordSize, i, n = atoi(argv[1]), send, recv;
    int vetor[n];
    int max = 0;
    int mim = rand();

    // Cria um vetor com valores randômicos.
    for( i = 0; i < n; i++ ){
        vetor[i] = rand() % 100;
    //    printf("Inseriu o valor %d no vetor.\n", vetor[i]);
    }
    MPI_Status status;

    MPI_Init(&argc, &argv);
    start = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //printf("Rank %d diz oi.\n", rank);

    MPI_Comm_size(MPI_COMM_WORLD, &wordSize);
    //printf("Quantidade de processos(nós) no comunicador é %d\n", wordSize);

    int elementos_por_no = n /(wordSize);
    //printf("Elementos por processos é %d\n", elementos_por_no);

    /*
     * O processo mestre(0) é responsável por enviar as partes do vetor
     * para os processos filhos realizarem a soma.
     */
    if( rank == MASTER )
    {
        // Percorre todos os processos filhos, enviando as mensagens empacotadas.
        for(i = 1; i < wordSize; i++)
        {
            send = elementos_por_no * i;
            //printf("Rank %d enviou send %d\n", rank, send);
            // Quantidade de elementos enviados para o processo i.
            MPI_Send(&send, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

        }

        // Processo mestre adiciona os elementos restantes no vetor e calcula o max e mim.
        for(i = 0; i < elementos_por_no; i++){
            som_parciais += vetor[i];
        }

        for(i = 0; i < n; i++){
            max = trocaMax(max, vetor[i]);
            mim = trocaMim(mim, vetor[i]);
        }

    }else
    {
        MPI_Recv(&recv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        // printf("%d recebeu %d from 0.\n", rank, recv);
        // Recebe a quantidade de elementos que precisa somar de sua parte do vetor.
        for( i = recv; i < (recv + elementos_por_no); i++ )
        {   
            //printf("Rank %d somando posição %d que tem o valor %d\n", rank, i, vetor[i]);
            soma_total += vetor[i];

        }
        //printf("Soma total do rank %d é: %d\n", rank, soma_total);
        MPI_Send(&soma_total, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
 
    if(rank == 0){
        for (rank = 1; rank < wordSize; rank++)
        {
            MPI_Recv(&soma_total, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, &status);
         //   printf("Rank 0 part 'two' soma parcial é %d\n", soma_total);
            som_parciais += soma_total;
        }
    }  

    end = MPI_Wtime();
    MPI_Finalize();

    // Verifica se é o último processo e mostra os resultados.
    if(rank == wordSize){
        // printf ("\nvetor:[");
        // for(int i = 0; i<n ; ++i){
        //     printf("%d ",vetor[i]);
        // }
        // printf("]\n");
        printf("\n\n#-----Informações referente a execução-----#\n\n");
        printf("Número de Processos: %d\n",wordSize);
        printf("Número de elementos no vetor: %d\n",n);
        printf("A soma dos elementos: %d\n", som_parciais);
        printf("A média é: %.4f\n", (float)som_parciais/n);
        printf("Valor Máximo: %d\n", max);
        printf("Valor Mínimo: %d\n", mim);
        printf("Total tempo de execução: %lf\n", end - start);
    }

    return 0;
}

int trocaMax(int a, int b){
    if(a < b){
        return b;
    }
    return a;
}

int trocaMim(int a, int b){
    if(a > b){
        return b;
    }
    return a;
}