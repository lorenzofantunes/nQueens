#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <unistd.h>

void initAnswer(int ** answer, int nSize){
    int i;
    for (i = 0; i < nSize * 2; i++, answer++) {
        //*answer =  i;
        *answer =  -1;
    }
}

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
int getX(int ** answer, int nSize, int level){
    int i, j;
    for (i = 0; i < nSize; i++, answer++, answer++) {
        if(i == level){
            return *answer;
        }
    }
}
int getY(int ** answer, int nSize, int level){
    int i, j;
    for (i = 0; i < nSize; i++, answer++, answer++) {
        if(i == level){
            answer++;
            return *answer;
        }
    }
}

void verify(int ** answer, int nSize, int level, int nThreads){
    int i, j;
        for (j = 0; j < nSize; j++) { //itera sobre cada casa (coluna) da linha atual
            //faz uma copia da resposta pra esse escopo
            int ** thisAnswer = malloc(sizeof(int *) * nSize * 2);
            memcpy(thisAnswer, answer, sizeof(int *) * nSize * 2);

            int flag = level + 1;
            //pega a posicao desse level
            for (i = 0; i < level + 1; i++) { //itera sobre os leveis anteriores para ver se nao da conflito
                int x = getX(thisAnswer, nSize, i); //o y vai ser o i ;-)
                if(
                    x != j && //não da conflito de coluna
                    ((level + 1) - j != i - x) && //nao da conflito da diagonal para direita
                    ((level + 1) + j != i + x)
                ){ //nao da conflito da diagonal para esquerda
                    flag--;
                }
            }
            if(flag == 0){
                setAnswer(thisAnswer, nSize, level+1, j);
                if(level+1 == nSize -1){ //ultimo level e n deu conflito
                    //printf("Resultado: ");
                    //printResultado(thisAnswer, nSize);
                    free(thisAnswer);

                }
                else {
                    verify(thisAnswer, nSize, level + 1, nThreads);
                }
            }
            else{
                free(thisAnswer);
            }
        }
}

void main(){
    int nSize, nThreads;
    double init, end;

    nSize = 12;
    nThreads = 8;

    printf("-------------------------------------AUTO-------------------------------------\n");
    int y, z;
    for (y = 1; y <= nThreads; y++) {
        printf("NÚMERO DE THREADS: %d\n", y);
        for (z = 4; z <= nSize; z++) {
            printf("Tabuleiro %dx%d \n", z, z);

            init = omp_get_wtime();
            #pragma omp parallel for schedule(static)
            for (size_t i = 0; i < nSize; i++) { //para cada casa em uma linha
                int ** answer = malloc(sizeof(int *) * z * 2);
                initAnswer(answer, z); //all less one
                setAnswer(answer, z, 0, i); //linha 0 coluna i
                verify(answer, z, 0, nThreads);
                free(answer);
            }
            end = omp_get_wtime();
            printf("%d threads: %lf\n",  y, end - init);
        }
        printf("||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
    }
}
