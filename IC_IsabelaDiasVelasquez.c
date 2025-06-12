int L = 8;
int N = 64;

int direita[64];  // N = 64, então os arrays devem ter tamanho 64
int esquerda[64];
int acima[64];
int abaixo[64];

int main() {
    for (int i = 0; i < N; ++i) {
        // Última coluna, descontamos L-1:
        if (i % L == L - 1) 
            direita[i] = i + 1 - L;
        else 
            direita[i] = i + 1;

        // Primeira coluna, somamos L-1:
        if (i % L == 0) 
            esquerda[i] = i - 1 + L;
        else 
            esquerda[i] = i - 1;

        // Primeira linha, somamos N-L:
        if (i < L) 
            acima[i] = i - L + N;
        else 
            acima[i] = i - L;

        // Última linha, módulo L:
        if (i >= N - L) 
            abaixo[i] = (i % L);
        else 
            abaixo[i] = i + L;
    }
    return 0;
}