#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Rede do sistema
#define L 50  //tamanho de cada lado da rede (quadrada)
#define N (L * L) //tamanho da rede

//Pontos
#define S 0.0 //C quando encontra D
#define T 1.4 //D quando encontra C
#define R 1.0 //C quando encontra C
#define P 0.0 //D quando encontra D
float pontos[N];

//Estratégias e posições iniciais
int s[N]; //s[i] = 0 se for D e s[i] = 1 se for C
int vizinhanca[N][4]; /*para cada i em N temos 4 vizinhos onde: 
[0] = direita; [1] = esquerda; [2] = acima; [3] = abaixo.*/

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

//Função pontuação inicial
void pts_iniciais(){
    for (int i = 0; i < N; ++i){
        pontos[i] = 0.0; //todos começam com 0 pontos (as interações não começaram)
    }
}

//Função random
float randi() {
    return ((float)rand() / (float)RAND_MAX);
}

//Função estratégias
void estrategias(){
    for (int i = 0; i < N; ++i){
        float ran = randi();
        if (ran < 0.5){
            s[i] = 0;
        } else {
            s[i] = 1;
        }
    }
}

//SIMULAÇÃO
int main() {
    srand(42); //semente para a função rand
    rede(); //rodando a rede
    pts_iniciais(); //pontuação incial de todos é 0
    estrategias(); //randomizando as estratégias

    //Interações
    for (int i = 0; i < N; ++i){
        if (s[i] == 0) { //se o agente é D
            for (int j = 0; j < 4; ++j){
                if (s[vizinhanca[i][j]] == 0){ //se o vizinho é D também
                    pontos[i] =+ P;
                    pontos[vizinhanca[i][j]] =+ P;
                }
                if (s[vizinhanca[i][j]] == 0){ //se o vizinho é C
                    pontos[i] =+ T;
                    pontos[vizinhanca[i][j]] =+ S;
                }
            }
        }
        if (s[i] == 1){ //se o agente é C
            for (int j = 0; j < 4; ++j){
                if (s[vizinhanca[i][j]] == 0){ //se o vizinho é D
                    pontos[i] =+ S;
                    pontos[vizinhanca[i][j]] =+ T;
                }
                if (s[vizinhanca[i][j]] == 0){ //se o vizinho é C também
                    pontos[i] =+ R;
                    pontos[vizinhanca[i][j]] =+ R;
                }
            }
        }
    }

    //Atualizando as estratégias
    for (int i = 0; i < N; ++i){
        for (int j = 0; j < 4; ++j) {
            if (pontos[i] < pontos[vizinhanca[i][j]]) {
                s[i] = s[vizinhanca[i][j]];
            }
        }
    }

    //PLOT
    FILE *gp;
    gp = popen("gnuplot -persist", "w");
    if (gp == NULL) {
        printf("Erro ao abrir o pipe para o Gnuplot.\n");
        exit(1);}
    
    fprintf(gp, "set title 'Gráfico da rede'\n");
    fprintf(gp, "set palette defined (0 'red', 1 'green')\n");
    fprintf(gp, "set pm3d map\n");
    fprintf(gp, "unset ztics\n");
    fprintf(gp, "splot '-' using 1:2:3 with points pt 7 palette\n");

    for (int i = 0; i < N; i++) {
        int cor;
        int x = i % L;
        int y = i / L;
        if (s[i] == 0) {
            cor = 0;
        } else if (s[i] == 1) {
            cor = 1;
        } fprintf(gp, "%d %d %d\n", x, y, cor);
    }
    
    fprintf(gp, "e\n");
    pclose(gp);

    return 0;

}