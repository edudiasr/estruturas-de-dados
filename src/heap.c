#include "heap.h"

#include <stdlib.h>

Heap *heap_criar(void) {
    return calloc(1, sizeof(Heap));
}

void heap_destruir(Heap *h) {
    if (!h) return;
    free(h->dados);
    free(h);
}

static void trocar(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

static void subir(int *v, size_t i) {
    while (i > 0 && v[(i - 1) / 2] > v[i]) {
        trocar(&v[(i - 1) / 2], &v[i]);
        i = (i - 1) / 2;
    }
}

/* Desce o valor da posição i enquanto ele for maior que algum filho.
 * `maior_primeiro` inverte a comparação (heap de máximo), usado pelo heapsort. */
static void descer(int *v, size_t n, size_t i, bool maior_primeiro) {
    for (;;) {
        size_t esq = 2 * i + 1, dir = esq + 1, escolhido = i;
        if (esq < n && (maior_primeiro ? v[esq] > v[escolhido] : v[esq] < v[escolhido])) escolhido = esq;
        if (dir < n && (maior_primeiro ? v[dir] > v[escolhido] : v[dir] < v[escolhido])) escolhido = dir;
        if (escolhido == i) return;
        trocar(&v[i], &v[escolhido]);
        i = escolhido;
    }
}

bool heap_inserir(Heap *h, int valor) {
    if (h->tamanho == h->capacidade) {
        size_t nova = h->capacidade ? h->capacidade * 2 : 8;
        int *dados = realloc(h->dados, nova * sizeof(int));
        if (!dados) return false;
        h->dados = dados;
        h->capacidade = nova;
    }
    h->dados[h->tamanho] = valor;
    subir(h->dados, h->tamanho++);
    return true;
}

bool heap_extrair_minimo(Heap *h, int *valor) {
    if (h->tamanho == 0) return false;
    *valor = h->dados[0];
    h->dados[0] = h->dados[--h->tamanho]; /* o último vai pro topo e desce até o lugar dele */
    descer(h->dados, h->tamanho, 0, false);
    return true;
}

bool heap_minimo(const Heap *h, int *valor) {
    if (h->tamanho == 0) return false;
    *valor = h->dados[0];
    return true;
}

void heapsort(int *v, size_t n) {
    if (n < 2) return;
    /* 1) transforma o vetor num heap de máximo, de baixo pra cima: O(n) */
    for (size_t i = n / 2; i-- > 0;) descer(v, n, i, true);
    /* 2) o maior está no topo: troca com o último e refaz o heap no resto */
    for (size_t fim = n - 1; fim > 0; fim--) {
        trocar(&v[0], &v[fim]);
        descer(v, fim, 0, true);
    }
}
