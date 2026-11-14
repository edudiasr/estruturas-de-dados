#include "arvore.h"

#include <stdlib.h>

Arvore *arvore_criar(void) {
    return calloc(1, sizeof(Arvore));
}

static void destruir_nos(NoArvore *n) {
    if (!n) return;
    destruir_nos(n->esquerda);
    destruir_nos(n->direita);
    free(n); /* filhos primeiro, depois o pai (pós-ordem) */
}

void arvore_destruir(Arvore *a) {
    if (!a) return;
    destruir_nos(a->raiz);
    free(a);
}

bool arvore_inserir(Arvore *a, int valor) {
    /* ponteiro pro ponteiro: aponta pro "lugar" onde o novo nó vai ser pendurado,
     * seja a raiz ou o campo esquerda/direita de algum nó. Evita tratar a raiz como caso especial. */
    NoArvore **lugar = &a->raiz;
    while (*lugar) {
        if (valor == (*lugar)->valor) return false;
        lugar = valor < (*lugar)->valor ? &(*lugar)->esquerda : &(*lugar)->direita;
    }
    NoArvore *n = calloc(1, sizeof(NoArvore));
    if (!n) return false;
    n->valor = valor;
    *lugar = n;
    a->tamanho++;
    return true;
}

bool arvore_contem(const Arvore *a, int valor) {
    const NoArvore *n = a->raiz;
    while (n) {
        if (valor == n->valor) return true;
        n = valor < n->valor ? n->esquerda : n->direita;
    }
    return false;
}

bool arvore_remover(Arvore *a, int valor) {
    NoArvore **lugar = &a->raiz;
    while (*lugar && (*lugar)->valor != valor) {
        lugar = valor < (*lugar)->valor ? &(*lugar)->esquerda : &(*lugar)->direita;
    }
    NoArvore *alvo = *lugar;
    if (!alvo) return false;

    if (!alvo->esquerda) {
        *lugar = alvo->direita;          /* 0 ou 1 filho: o filho (ou NULL) sobe pro lugar dele */
    } else if (!alvo->direita) {
        *lugar = alvo->esquerda;
    } else {
        /* 2 filhos: o sucessor (menor valor da subárvore direita) toma o lugar.
         * Ele não tem filho à esquerda, então sair de onde está é o caso simples. */
        NoArvore **suc = &alvo->direita;
        while ((*suc)->esquerda) suc = &(*suc)->esquerda;
        NoArvore *sucessor = *suc;
        *suc = sucessor->direita;
        sucessor->esquerda = alvo->esquerda;
        sucessor->direita = alvo->direita;
        *lugar = sucessor;
    }
    free(alvo);
    a->tamanho--;
    return true;
}

bool arvore_minimo(const Arvore *a, int *valor) {
    const NoArvore *n = a->raiz;
    if (!n) return false;
    while (n->esquerda) n = n->esquerda;
    *valor = n->valor;
    return true;
}

bool arvore_maximo(const Arvore *a, int *valor) {
    const NoArvore *n = a->raiz;
    if (!n) return false;
    while (n->direita) n = n->direita;
    *valor = n->valor;
    return true;
}

static int altura(const NoArvore *n) {
    if (!n) return 0;
    int e = altura(n->esquerda), d = altura(n->direita);
    return 1 + (e > d ? e : d);
}

int arvore_altura(const Arvore *a) {
    return altura(a->raiz);
}

static void em_ordem(const NoArvore *n, int *saida, size_t max, size_t *i) {
    if (!n || *i >= max) return;
    em_ordem(n->esquerda, saida, max, i);
    if (*i < max) saida[(*i)++] = n->valor;
    em_ordem(n->direita, saida, max, i);
}

size_t arvore_em_ordem(const Arvore *a, int *saida, size_t max) {
    size_t i = 0;
    em_ordem(a->raiz, saida, max, &i);
    return i;
}
