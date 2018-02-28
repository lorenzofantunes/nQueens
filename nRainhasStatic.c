#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <unistd.h>

//inicia o vetor de respostas
void initAnswer(int ** answer, int nSize){
    int i;
    for (i = 0; i < nSize * 2; i++, answer++) {
        //*answer =  i;
        *answer =  -1;
    }
}

//define um valor em algum level do valor de respostas
void setAnswer(int ** answer, int nSize, int level, int x){
    int i, j;
    for (i = 0; i < nSize; i++, answer++, answer++) {
        if(i == level){
            *answer = x;
            answer++;
            *answer = level;
            break;
        }
    }
}

//escreve os resultados na tela
void printResultado(int ** answer, int nSize){
    int j;
    #pragma omp critical
    {
        for (j = 0; j < nSize; j++, answer++) {
            int value = *(answer);
            answer++;
            printf("(%d - %d), ", value, *(answer));
        }
        printf("\n");
    }
}

//pega o valor de x de algum level
int getX(int ** answer, int nSize, int level){
    int i, j;
    for (i = 0; i < nSize; i++, answer++, answer++) {
        if(i == level){
            return *answer;
        }
    }
}

//verifica quais posicoes estao livres e verifica se eh uma resposta valida
void verify(int ** answer, int nSize, int level, int nThreads){
    int i, j;
    for (j = 0; j < nSize; j++) { //itera sobre cada casa (coluna) da linha atual
        //faz uma copia da resposta pra esse escopo
        int ** thisAnswer = malloc(sizeof(int *) * nSize * 2);
        memcpy(thisAnswer, answer, sizeof(int *) * nSize * 2);

        //pega a posicao desse level
        int flag = level + 1;
        for (i = 0; i < level + 1; i++) { //itera sobre os leveis anteriores para ver se nao da conflito
            int x = getX(thisAnswer, nSize, i); //o y vai ser o i ;-)
            if(
                x != j && //não da conflito de coluna
                ((level + 1) - j != i - x) && //nao da conflito da diagonal para direita
                ((level + 1) + j != i + x) //nao da conflito da diagonal para esquerda
            ){
                flag--; //removendo leveis sem conflito
            }
        }
        if(flag == 0){
            //resultado valido para aquela linha
            setAnswer(thisAnswer, nSize, level+1, j); //atualiza o valor do vetor de respostas

            if(level+1 == nSize -1){ //ultimo level e n deu conflito
                //printf("Resultado: ");
                //printResultado(thisAnswer, nSize); //para printar os resultados so descomentar
                free(thisAnswer);
            }
            else {
                verify(thisAnswer, nSize, level + 1, nThreads); //se nao estiver no level final entra mais na recursao
            }
        }
        else{
            //se nao for um resultado valido
            free(thisAnswer);
        }
    }
}

void main(){
    int nSize, nThreads;
    double init, end;

    nSize = 12;
    nThreads = 8;

    int y, z;

    //itera sobre o numero de threads, passa para cada thread uma das posicoes do tabuleiro para iniciar
    printf("-------------------------------------STATIC-------------------------------------\n");
    for (y = 1; y <= nThreads; y++) {
        printf("NÚMERO DE THREADS: %d\n", y);
        for (z = 4; z <= nSize; z++) {
            printf("Tabuleiro %dx%d \n", z, z);

            init = omp_get_wtime();
            #pragma omp parallel for schedule(static) num_threads(y)
            for (size_t i = 0; i < nSize; i++) { //para cada casa em uma linha
                int ** answer = malloc(sizeof(int *) * z * 2);
                initAnswer(answer, z); //all less one
                setAnswer(answer, z, 0, i); //linha 0 coluna i
                verify(answer, z, 0, nThreads); //entra na recursao para os proximos niveis
                free(answer);
            }
            end = omp_get_wtime();
            printf("%d threads: %lf\n",  y, end - init);
        }
        printf("||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
    }
}
