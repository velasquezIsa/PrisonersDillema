/***************************************************************************************
*                        Dilema do Prisioneiro - Densidade Variável                    *
*                                         15/11/2025                                   *
***************************************************************************************/
//gcc -o 'L100_RHO(0-1)_M0' 'L100_RHO(0-1)_M0.c' -O3 -lgsl -lgslcblas -lm

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <gsl/gsl_rng.h>

/****************************************************************************************
*                                  Definição de Constantes                              *
*                                                                                       *
****************************************************************************************/

#define L 100                 //tamanho de cada lado da rede (quadrada)
#define N (L * L)             //tamanho da rede
#define MCS 10000             //tempo total de simulação

#define SAVE_CONFIG 0         //salvar a configuração final
#define SAVE_TEMP 1          //salvar pc em relação ao tempo

/***************************************************************************************
*                                  Declaração de Funções                               *
*                                                                                      *
***************************************************************************************/

//GSL
gsl_rng *r;
void init_rng(unsigned long int seed) {
    const gsl_rng_type * T = gsl_rng_mt19937; 
    r = gsl_rng_alloc(T);
    gsl_rng_set(r, seed);
}
void liberar_rng() {gsl_rng_free(r);}

//Rede do sistema
float rho; //densidade teórica da rede
float n; //número de agentes na rede
float p[21]; //densidade real total da rede
float pcp[21]; //densidade de cooperadores pela densidade da rede
float pcp_tempo[21][21]; //densidade de cooperadores pela densidade da rede em relação ao tempo

//Tempo em MCS
int t[21][21];
//Pontos
#define S 0.0                 //pontuação de C quando encontra D
#define TT 1.4                //pontuação de D quando encontra C
#define R 1.0                 //pontuação de C quando encontra C
#define P 0.0                 //pontuação de D quando encontra D
float pontos[N];

//Estratégias e posições iniciais
int s[N]; //s[i] = 0 se for D, s[i] = 1 se for C e s[i] = 2 se o sítio for vazio
int vizinhanca[N][4]; /*para cada i em N temos 4 vizinhos onde: [0] = direita; [1] = esquerda; [2] = acima; [3] = abaixo.*/

//Rede de vizinhança
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

//Pontuação inicial
void pts_iniciais(){for (int i = 0; i < N; ++i){pontos[i] = 0.0;}} //todos começam com 0 pontos em cada rodada

//Números aleatórios entre 0 e 1
float randi() {return (float)gsl_rng_uniform(r);}

//Estratégias em função da densidade da rede
void estrategias(){
    n = 0.0;
    for (int i = 0; i < N; i++){
        if (randi() < rho) {
          n += 1;
          if (randi() < 0.5) s[i] = 0;
          else s[i] = 1;} 
        else {s[i] = 2;}
    }
}

//Densidade de C em N
float densidade(){
    float soma_C = 0.0;
    for (int k = 0; k < N; k++) {
        if (s[k] == 1) {soma_C += 1.0;}
    }
    return soma_C/N;}

/******************************************************************************************
*                               FUNÇÕES DE ARQUIVOS DE DADOS                              *
*                                                                                         *
******************************************************************************************/

//Função para a seed inicial
unsigned long funcSeed(){
  unsigned long seed = (unsigned long)time(NULL);
  if (seed%2 == 0) seed += 1; //semente é ímpar
  return seed;
  }

FILE*fp1;
char name[100];
int ok = 0;
//Salvar os dados de pc e pcp
void saveConfig(unsigned long seed){
  if (SAVE_CONFIG == 1){
    while (ok == 0){
      sprintf(name,"SDP_L%d_RHO(0-1)_M0_S%lu.dat",L,seed);
      FILE *temp_fp = fopen(name, "r");
      if (temp_fp != NULL){seed += 1; fclose(temp_fp); ok = 0;}
      else ok = 1;}
    fp1 = fopen(name,"w");
    fprintf(fp1,"# Simulação do Dilema do Prisioneiro\n");
    fprintf(fp1,"# Comprimento da rede: %d\n",L);
    fprintf(fp1,"# RHO: (0-1)\n");
    fprintf(fp1,"# Tempo máximo de simulação: %d\n", MCS);
    fprintf(fp1,"# seed rho p pcp\n"); }
  else fp1 = stdout;}

//Salvar pcp_tempo e tempo
int saveTemp(unsigned long seed){
  if (SAVE_TEMP == 1){
    while (ok == 0){
      sprintf(name,"SDP_L%d_RHO(0-1)_M0_S%lu.dat",L,seed);
      FILE *temp_fp = fopen(name, "r");
      if (temp_fp != NULL){seed += 1; fclose(temp_fp); ok = 0;}
      else ok = 1;}
    fp1 = fopen(name,"w");
    fprintf(fp1,"# Simulação do Dilema do Prisioneiro - rho em relação ao tempo\n");
    fprintf(fp1,"# Comprimento da rede: %d\n",L);
    fprintf(fp1,"# RHO: %.2f\n", rho);
    fprintf(fp1,"# Tempo máximo de simulação: %d\n", MCS);
    fprintf(fp1,"# seed rho p t pcp\n"); }
  else fp1 = stdout;}

/*****************************************************************************************************************************
*                                                        Simulação                                                           *
*****************************************************************************************************************************/
int main(){
  unsigned long seed = funcSeed(); //define a semente de números aleatórios
    
  saveConfig(seed); //Funções de registro de dados
  saveTemp(seed);
  
  init_rng(seed);
  
  
  p[0] = 0;
  pcp[0] = 0;
  for (int i = 0; i < 21; ++i){t[0][i] = 0; pcp_tempo[0][i] = 0;}
  rede(); //montando a rede

/********************************* Interações ******************************************************************************/
  for (int l = 1; l < 21; ++l){ //loop para determinar a densidade teórica
    rho = l/20.0;
    estrategias();
    for (int k = 0; k < MCS; ++k) { //loop para as interações
      pts_iniciais(); 
      if(SAVE_TEMP == 1 && k == 0){
        t[l][0] = k;
        pcp_tempo[l][0] = densidade();}
        
      for (int i = 0; i < N; ++i){ //jogo de cada jogador N
      
        if (s[i] == 0) { //se o agente é D
          for (int j = 0; j < 4; ++j){
            if (s[vizinhanca[i][j]] == 0){ //se o vizinho é D também
              pontos[i] += P;}
            if (s[vizinhanca[i][j]] == 1){ //se o vizinho é C
                  pontos[i] += TT;}
          }
        }
        if (s[i] == 1){ //se o agente é C
          for (int j = 0; j < 4; ++j){
            if (s[vizinhanca[i][j]] == 0){ //se o vizinho é D
              pontos[i] += S;}
            if (s[vizinhanca[i][j]] == 1){ //se o vizinho é C também
              pontos[i] += R;}
          }
        }
      }
      
    int s_novo[N]; //atualização de estratégias ao final de cada rodada vvvv
    for (int i = 0; i < N; ++i){
      if (s[i] < 2){
        int agente = i;
        int j0 = randi() * 4;
        float ponto = pontos[i];
        for (int j = 0; j < 4; ++j){
          if (ponto < pontos[vizinhanca[i][j0]]) {
            ponto = pontos[vizinhanca[i][j0]];
            agente = vizinhanca[i][j0];}
          j0 = (j0 + 1) % 4;}
        s_novo[i] = s[agente];}
      else s_novo[i] = 2;
    }
    for (int i = 0; i < N; ++i) {s[i] = s_novo[i];} 
    
    p[l] = n/N;
    if(SAVE_TEMP == 1 && (k + 1) % 500 == 0){
      t[l][(k + 1)/500] = (k + 1);
      pcp_tempo[l][(k + 1)/500] = densidade()/p[l];} } 
    
/*********************************** Recolhendo dados no final da simulação **************************************************/
  if (SAVE_CONFIG == 1){pcp[l] = densidade()/p[l];  fprintf(fp1,"%lu %.4f %.4f %.4f\n", seed, rho, p[l], pcp[l]);}
  if (SAVE_TEMP == 1){for (int j = 0; j < 21; ++j)fprintf(fp1,"%lu %.4f %.4f %d %.4f\n", seed, rho, p[l], t[l][j], pcp_tempo[l][j]);}
  }

  if (SAVE_CONFIG == 1 || SAVE_TEMP == 1) fclose(fp1);
  liberar_rng();
  return 0;
}
