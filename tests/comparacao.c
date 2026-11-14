/* Mede na prática o que a tabela de complexidade diz: buscar em lista é O(n), em árvore de
 * busca é O(log n) e em tabela hash é O(1) em média.
 *
 *   ./comparacao        (n = 1.000, 10.000 e 100.000) */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../src/arvore.h"
#include "../src/hash.h"
#include "../src/lista.h"

static double agora(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

/* embaralha 0..n-1 (Fisher-Yates) pra árvore não receber os números em ordem */
static int *embaralhados(int n) {
    int *v = malloc((size_t)n * sizeof(int));
    for (int i = 0; i < n; i++) v[i] = i;
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1), t = v[i];
        v[i] = v[j];
        v[j] = t;
    }
    return v;
}

int main(void) {
    srand(7);
    const int BUSCAS = 2000;
    printf("%10s %16s %16s %16s\n", "n", "lista", "árvore", "hash");
    for (int n = 1000; n <= 100000; n *= 10) {
        int *valores = embaralhados(n);
        Lista *l = lista_criar();
        Arvore *a = arvore_criar();
        Hash *h = hash_criar();
        char chave[16];
        for (int i = 0; i < n; i++) {
            lista_inserir_fim(l, valores[i]);
            arvore_inserir(a, valores[i]);
            snprintf(chave, sizeof chave, "%d", valores[i]);
            hash_definir(h, chave, valores[i]);
        }

        volatile long achou = 0; /* volatile: o compilador não pode jogar a busca fora */
        double t0 = agora();
        for (int i = 0; i < BUSCAS; i++) achou += lista_buscar(l, rand() % n) >= 0;
        double t1 = agora();
        for (int i = 0; i < BUSCAS; i++) achou += arvore_contem(a, rand() % n);
        double t2 = agora();
        int v;
        for (int i = 0; i < BUSCAS; i++) {
            snprintf(chave, sizeof chave, "%d", rand() % n);
            achou += hash_obter(h, chave, &v);
        }
        double t3 = agora();

        printf("%10d %13.2f us %13.2f us %13.2f us   (altura da árvore: %d)\n", n,
               (t1 - t0) / BUSCAS * 1e6, (t2 - t1) / BUSCAS * 1e6, (t3 - t2) / BUSCAS * 1e6, arvore_altura(a));
        lista_destruir(l);
        arvore_destruir(a);
        hash_destruir(h);
        free(valores);
    }
    return 0;
}
