#include "pilha_fila.h"

#include <stdlib.h>
#include <string.h>

#define CAPACIDADE_INICIAL 4

/* ------------------------------------------------------------------ pilha */

Pilha *pilha_criar(void) {
    return calloc(1, sizeof(Pilha));
}

void pilha_destruir(Pilha *p) {
    if (!p) return;
    free(p->dados);
    free(p);
}

bool pilha_empilhar(Pilha *p, int valor) {
    if (p->tamanho == p->capacidade) {
        size_t nova = p->capacidade ? p->capacidade * 2 : CAPACIDADE_INICIAL;
        int *dados = realloc(p->dados, nova * sizeof(int));
        if (!dados) return false; /* sem memória: a pilha antiga continua intacta */
        p->dados = dados;
        p->capacidade = nova;
    }
    p->dados[p->tamanho++] = valor;
    return true;
}

bool pilha_desempilhar(Pilha *p, int *valor) {
    if (p->tamanho == 0) return false;
    *valor = p->dados[--p->tamanho];
    return true;
}

bool pilha_topo(const Pilha *p, int *valor) {
    if (p->tamanho == 0) return false;
    *valor = p->dados[p->tamanho - 1];
    return true;
}

/* ------------------------------------------------------------------ fila */

Fila *fila_criar(void) {
    return calloc(1, sizeof(Fila));
}

void fila_destruir(Fila *f) {
    if (!f) return;
    free(f->dados);
    free(f);
}

static bool fila_crescer(Fila *f) {
    size_t nova = f->capacidade ? f->capacidade * 2 : CAPACIDADE_INICIAL;
    int *dados = malloc(nova * sizeof(int));
    if (!dados) return false;
    /* copia na ordem da fila (a parte do inicio até o fim do vetor, depois a que deu a volta) */
    for (size_t i = 0; i < f->tamanho; i++) {
        dados[i] = f->dados[(f->inicio + i) % f->capacidade];
    }
    free(f->dados);
    f->dados = dados;
    f->capacidade = nova;
    f->inicio = 0;
    return true;
}

bool fila_enfileirar(Fila *f, int valor) {
    if (f->tamanho == f->capacidade && !fila_crescer(f)) return false;
    f->dados[(f->inicio + f->tamanho) % f->capacidade] = valor;
    f->tamanho++;
    return true;
}

bool fila_desenfileirar(Fila *f, int *valor) {
    if (f->tamanho == 0) return false;
    *valor = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % f->capacidade;
    f->tamanho--;
    return true;
}

bool fila_frente(const Fila *f, int *valor) {
    if (f->tamanho == 0) return false;
    *valor = f->dados[f->inicio];
    return true;
}

/* ------------------------------------------------------------------ aplicação */

bool expressao_balanceada(const char *texto) {
    Pilha *p = pilha_criar();
    if (!p) return false;
    bool ok = true;
    for (const char *c = texto; *c && ok; c++) {
        if (strchr("([{", *c)) {
            ok = pilha_empilhar(p, *c);
        } else if (strchr(")]}", *c)) {
            int aberto;
            char esperado = *c == ')' ? '(' : *c == ']' ? '[' : '{';
            /* fecha sem ter aberto, ou fecha com o tipo errado: "(]" */
            ok = pilha_desempilhar(p, &aberto) && aberto == esperado;
        }
    }
    ok = ok && p->tamanho == 0; /* sobrou algo aberto: "((" */
    pilha_destruir(p);
    return ok;
}
