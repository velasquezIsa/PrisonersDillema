#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lat2eps.h"

//Nome do arquivo gerado
char name[100];

//Rede do sistema
#define L 100  //tamanho de cada lado da rede (quadrada)
#define N (L * L) //tamanho da rede
float rho = 1;

//Tempo em MCS
#define MCS 10000
int tempo = 0;

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
    float ran;
    for (int i = 0; i < N; ++i){
        ran = randi();
        if (ran < 0.5) {
            s[i] = 0;
        } else if (ran >= 0.5) {
            s[i] = 1;
        }
    }
}

//Função densidade de C em N
int densidade(){
    float soma_C = 0.0;
    float rho_c = 0.0;
    for (int k = 0; k < N; ++k) {
        if (s[k] == 1) {
            soma_C += 1.0;
        }
    }
    rho_c = soma_C/N;
    printf("p_c/p em t = %d: %f\n", tempo, rho_c);
}

//Função PLOT
void plot_lat(const char *name){
    lat2eps_init(L,L);
    lat2eps_set_color(2,0xFFFFFF); // white
    lat2eps_set_color(1,0x0115b2); // blue
    lat2eps_set_color(0,0xFF0000); // red
    for (int j=0; j<N; ++j) {
        if (s[j] == 1) 
            lat2eps_set_site(j%L,(L - 1) - (j / L),1);
        else if (s[j] == 0) 
            lat2eps_set_site(j%L,(L - 1) - (j / L),0);}

    lat2eps_gen_eps(name,0,0,L,L,2,6);
    lat2eps_release();
}








//SIMULAÇÃO
int main() {
    srand(42); //semente para a função rand
    rede(); //montando a rede
    pts_iniciais(); //pontuação incial de todos é 0
    estrategias(); //randomizando as estratégias
    densidade(); //densidade de C's no início da simulação
    snprintf(name, sizeof(name), "m0_p1_t%d.eps", tempo);
    plot_lat(name); //plotando o estado inicial da rede

    //Interações
    for (int k = 0; k < MCS; ++k) {
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
    }

    //Atualizando as estratégias
    int s_novo[N];
    for (int i = 0; i < N; ++i) {
        s_novo[i] = s[i];
    }
    for (int i = 0; i < N; ++i){
        int j0 = randi() * 4;
        for (int j = 0; j < 4; ++j){
            if (pontos[i] < pontos[vizinhanca[i][j0]]){
                s_novo[i] = s[vizinhanca[i][j0]];
            }
            j0 = (j0 + 1) % 4;
        }
    }
    for (int i = 0; i < N; ++i) {
        s[i] = s_novo[i];
    }

    //Recolhendo dados no final da simulação
    tempo = MCS;
    snprintf(name, sizeof(name), "m0_p1_t%d.eps", tempo);
    densidade();
    plot_lat(name);

    return 0;
}
