#include <stdio.h>
#include <math.h>

#define L 8  //tamanho de cada lado da rede (quadrada)
#define N (L * L) //tamanho da rede


int s[1]; //s[0] = D e s[1] = C
int vizinhanca[N][3]; //para cada i em N temos 4 vizinhos onde: [0] = direita; [1] = esquerda; [2] = acima; [3] = abaixo.

void rede(){
    for (int i = 0; i < N; ++i) {
            if (i % L == L - 1) 
                vizinhanca[i][0] = i + 1 - L;
            else 
                vizinhanca[i][0] = i + 1;

            if (i % L == 0) 
                vizinhanca[i][1] = i - 1 + L;
            else 
                vizinhanca[i][1] = i - 1;

            if (i < L) 
                vizinhanca[i][2] = i - L + N;
            else 
                vizinhanca[i][2] = i - L;

            if (i >= N - L) 
                vizinhanca[i][3] = (i % L);
            else 
                vizinhanca[i][3] = i + L;
        }
    }

int seed = 42;
int randi() {
    int a = 1664525;
    int b = 1013904223;
    long long int c = 4294967296LL; 

    float seed1;
    seed = (a*seed +b) % c
    seed = seed/c
    return seed;
}

int main() {
    rede();

    int pontos[N];
    for (int i = 0; i < N; ++i){
        for (int j = 0; j < 4; j++){
        }
        if (pontos[i] < pontos[vizinhanca[i][0]])
            s[i] = s[vizinhanca[i][0]];

        if (pontos[i] < pontos[vizinhanca[i][1]])
            s[i] = s[vizinhanca[i][1]];
        
        if (pontos[i] < pontos[vizinhanca[i][2]])
            s[i] = s[vizinhanca[i][2]];
        
        if (pontos[i] < pontos[vizinhanca[i][3]])
            s[i] = s[vizinhanca[i][3]];
        
    }
    return 0;
}