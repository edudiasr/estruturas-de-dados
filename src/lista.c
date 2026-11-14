#include "lista.h"

#include <stdlib.h>

Lista *lista_criar(void) {
    return calloc(1, sizeof(Lista));
}

void lista_destruir(Lista *l) {
    if (!l) return;
    No *atual = l->inicio;
    while (atual) {
        No *proximo = atual->proximo; /* guarda antes de liberar: depois do free não dá mais pra ler */
        free(atual);
        atual = proximo;
    }
    free(l);
}

static No *novo_no(int valor) {
    No *n = malloc(sizeof(No));
    if (n) {
        n->valor = valor;
        n->anterior = n->proximo = NULL;
    }
    return n;
}

bool lista_inserir_inicio(Lista *l, int valor) {
    No *n = novo_no(valor);
    if (!n) return false;
    n->proximo = l->inicio;
    if (l->inicio) l->inicio->anterior = n;
    else l->fim = n; /* lista estava vazia: o novo é início e fim */
    l->inicio = n;
    l->tamanho++;
    return true;
}

bool lista_inserir_fim(Lista *l, int valor) {
    No *n = novo_no(valor);
    if (!n) return false;
    n->anterior = l->fim;
    if (l->fim) l->fim->proximo = n;
    else l->inicio = n;
    l->fim = n;
    l->tamanho++;
    return true;
}

/* Nó da posição pos, andando pelo lado mais perto (início ou fim). */
static No *no_em(const Lista *l, size_t pos) {
    if (pos < l->tamanho / 2) {
        No *n = l->inicio;
        for (size_t i = 0; i < pos; i++) n = n->proximo;
        return n;
    }
    No *n = l->fim;
    for (size_t i = l->tamanho - 1; i > pos; i--) n = n->anterior;
    return n;
}

bool lista_inserir_em(Lista *l, size_t pos, int valor) {
    if (pos > l->tamanho) return false;
    if (pos == 0) return lista_inserir_inicio(l, valor);
    if (pos == l->tamanho) return lista_inserir_fim(l, valor);

    No *depois = no_em(l, pos);
    No *n = novo_no(valor);
    if (!n) return false;
    n->anterior = depois->anterior;
    n->proximo = depois;
    depois->anterior->proximo = n;
    depois->anterior = n;
    l->tamanho++;
    return true;
}

static void desligar(Lista *l, No *n) {
    if (n->anterior) n->anterior->proximo = n->proximo;
    else l->inicio = n->proximo;
    if (n->proximo) n->proximo->anterior = n->anterior;
    else l->fim = n->anterior;
    free(n);
    l->tamanho--;
}

bool lista_remover_inicio(Lista *l, int *valor) {
    if (!l->inicio) return false;
    if (valor) *valor = l->inicio->valor;
    desligar(l, l->inicio);
    return true;
}

bool lista_remover_fim(Lista *l, int *valor) {
    if (!l->fim) return false;
    if (valor) *valor = l->fim->valor;
    desligar(l, l->fim);
    return true;
}

bool lista_remover_valor(Lista *l, int valor) {
    for (No *n = l->inicio; n; n = n->proximo) {
        if (n->valor == valor) {
            desligar(l, n);
            return true;
        }
    }
    return false;
}

bool lista_obter(const Lista *l, size_t pos, int *valor) {
    if (pos >= l->tamanho) return false;
    *valor = no_em(l, pos)->valor;
    return true;
}

long lista_buscar(const Lista *l, int valor) {
    long pos = 0;
    for (No *n = l->inicio; n; n = n->proximo, pos++) {
        if (n->valor == valor) return pos;
    }
    return -1;
}

void lista_inverter(Lista *l) {
    /* troca anterior <-> proximo em cada nó e depois troca início <-> fim */
    for (No *n = l->inicio; n; n = n->anterior) { /* depois da troca, o "anterior" é o antigo próximo */
        No *tmp = n->proximo;
        n->proximo = n->anterior;
        n->anterior = tmp;
    }
    No *tmp = l->inicio;
    l->inicio = l->fim;
    l->fim = tmp;
}
