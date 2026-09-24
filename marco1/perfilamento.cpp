/* Marco 1 — Perfilamento do baseline 07 (mergesort)
 * IESB 2026/2 — CCO085
 *
 * Copia instrumentada de 07_mergesort.cpp: mesma logica de ordenacao e mesma
 * entrada (semente 12345), com timers nos trechos abaixo:
 *   1. tempo total do mergesort (ja incluindo o custo dos timers);
 *   2. tempo da 1a divisao em [lista_1, lista_2] ate a lista_2 comecar a ser dividida;
 *   3. pior tempo de um unico merge;
 *   4. soma do tempo de todos os merges.
 * Para cada tamanho, roda uma vez para aquecimento (descartada) e depois
 * RODADAS vezes, imprimindo a media de cada trecho.
 *
 * Compilar e executar: make perfilamento
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cstdint>
#include <ctime>
using namespace std;

static const int TAMANHOS[] = {50000, 100000, 1000000};
static const int QTD_TAMANHOS = 3;
static const int RODADAS = 3;  // rodadas medidas, alem da primeira, que e descartada

static double agora() {
    timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

struct Medidas {
    double total = 0.0, ate_lista_2 = 0.0, pior_merge = 0.0, soma_merges = 0.0;
    long merges = 0;
};

static Medidas medidas;

static void merge(vector<int>& a, vector<int>& tmp, int lo, int mid, int hi) {
    double t0 = agora();
    int i = lo, j = mid, k = lo;
    while (i < mid && j < hi) tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi)  tmp[k++] = a[j++];
    for (int t = lo; t < hi; t++) a[t] = tmp[t];
    double t = agora() - t0;
    if (t > medidas.pior_merge) medidas.pior_merge = t;
    medidas.soma_merges += t;
    medidas.merges++;
}
static void msort(vector<int>& a, vector<int>& tmp, int lo, int hi) {
    if (hi - lo <= 1) return;
    int mid = (lo + hi) / 2;
    msort(a, tmp, lo, mid);
    msort(a, tmp, mid, hi);
    merge(a, tmp, lo, mid, hi);
}

/* Uma execucao completa do mergesort instrumentado sobre n numeros. */
static Medidas perfilar(int n) {
    vector<int> a(n), tmp(n);
    uint64_t s = 12345;
    for (int i = 0; i < n; i++) { s = s*6364136223846793005ULL + 1; a[i] = (int)(s >> 33); }

    medidas = Medidas();

    // Primeira divisao feita aqui (mesmo passo de msort) para marcar os trechos do nivel mais alto
    double t0 = agora();
    int lo = 0, hi = n, mid = (lo + hi) / 2;
    msort(a, tmp, lo, mid);
    // Trecho 2: da primeira divisao ate a lista_2 comecar a ser dividida
    medidas.ate_lista_2 = agora() - t0;
    msort(a, tmp, mid, hi);
    merge(a, tmp, lo, mid, hi);
    // Trecho 1: tempo total, ja incluindo o custo dos timers
    medidas.total = agora() - t0;

    for (int i = 1; i < n; i++) {
        if (a[i-1] > a[i]) { fprintf(stderr, "erro: n=%d nao ficou ordenado\n", n); exit(1); }
    }
    return medidas;
}

/* Descarta a primeira rodada (aquecimento) e tira a media das RODADAS seguintes. */
static Medidas media_das_rodadas(int n) {
    perfilar(n);
    Medidas soma;
    for (int r = 0; r < RODADAS; r++) {
        Medidas m = perfilar(n);
        soma.total += m.total;
        soma.ate_lista_2 += m.ate_lista_2;
        soma.pior_merge += m.pior_merge;
        soma.soma_merges += m.soma_merges;
        soma.merges += m.merges;
    }
    soma.total /= RODADAS;
    soma.ate_lista_2 /= RODADAS;
    soma.pior_merge /= RODADAS;
    soma.soma_merges /= RODADAS;
    soma.merges /= RODADAS;
    return soma;
}

/* Imprime o texto e completa com espacos ate a largura (conta caracteres UTF-8, nao bytes). */
static void imprimir_coluna(const char* texto, int largura) {
    int caracteres = 0;
    for (const char* p = texto; *p; p++) if ((*p & 0xC0) != 0x80) caracteres++;
    printf("| %s%*s", texto, largura - caracteres, "");
}

int main() {
    Medidas res[QTD_TAMANHOS];
    for (int i = 0; i < QTD_TAMANHOS; i++) res[i] = media_das_rodadas(TAMANHOS[i]);

    const char* nomes[] = {
        "1. Tempo total do merge sort",
        "2. Da 1ª divisão até começar a dividir a lista_2",
        "3. Pior tempo de um único merge",
        "4. Soma de todos os merges",
        "   Quantidade de merges",
    };
    const int LARGURA = 49;

    char separador[256] = "+--------------------------------------------------+";
    for (int i = 0; i < QTD_TAMANHOS; i++) strcat(separador, "-----------------+");

    printf("Média de %d rodadas (a primeira rodada, de aquecimento, foi descartada)\n\n", RODADAS);
    printf("%s\n", separador);
    imprimir_coluna("Trecho", LARGURA);
    for (int i = 0; i < QTD_TAMANHOS; i++) {
        char titulo[32];
        snprintf(titulo, sizeof(titulo), "n = %d", TAMANHOS[i]);
        printf("| %15s ", titulo);
    }
    printf("|\n%s\n", separador);

    for (int t = 0; t < 5; t++) {
        imprimir_coluna(nomes[t], LARGURA);
        for (int i = 0; i < QTD_TAMANHOS; i++) {
            const Medidas& m = res[i];
            switch (t) {
                case 0: printf("| %12.3f ms ", m.total * 1000); break;
                case 1: printf("| %12.3f ms ", m.ate_lista_2 * 1000); break;
                case 2: printf("| %12.3f ms ", m.pior_merge * 1000); break;
                case 3: printf("| %12.3f ms ", m.soma_merges * 1000); break;
                case 4: printf("| %15ld ", m.merges); break;
            }
        }
        printf("|\n");
    }
    printf("%s\n", separador);
    return 0;
}
