#ifndef PILHA_FILA_H
#define PILHA_FILA_H

#include <stdbool.h>
#include <stddef.h>

/* Pilha (último a entrar, primeiro a sair) num vetor que dobra de tamanho quando enche.
 * Empilhar é O(1) "amortizado": de vez em quando custa O(n) pra copiar, mas na média dá O(1). */
typedef struct {
    int *dados;
    size_t tamanho;
    size_t capacidade;
} Pilha;

Pilha *pilha_criar(void);
void pilha_destruir(Pilha *p);
bool pilha_empilhar(Pilha *p, int valor);
bool pilha_desempilhar(Pilha *p, int *valor);
bool pilha_topo(const Pilha *p, int *valor);

/* Fila (primeiro a entrar, primeiro a sair) num vetor circular:
 *
 *   [ . . 5 8 2 . ]      inicio aponta pro 5; quando chega no fim do vetor, volta pro começo
 *         ^inicio
 *
 * Tirar da frente é O(1) sem mover ninguém, ao contrário de um vetor comum. */
typedef struct {
    int *dados;
    size_t inicio;
    size_t tamanho;
    size_t capacidade;
} Fila;

Fila *fila_criar(void);
void fila_destruir(Fila *f);
bool fila_enfileirar(Fila *f, int valor);
bool fila_desenfileirar(Fila *f, int *valor);
bool fila_frente(const Fila *f, int *valor);

/* Aplicação de pilha: confere se ( ) [ ] { } estão balanceados numa expressão. */
bool expressao_balanceada(const char *texto);

#endif
