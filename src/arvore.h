#ifndef ARVORE_H
#define ARVORE_H

#include <stdbool.h>
#include <stddef.h>

/* Árvore binária de busca: à esquerda de cada nó só tem menores, à direita só maiores.
 *
 *          8
 *        /   \
 *       3     10
 *      / \      \
 *     1   6      14
 *
 * Buscar, inserir e remover descem um caminho da raiz até uma folha: O(altura).
 * Com inserções em ordem aleatória a altura fica perto de log2(n); inserindo já ordenado
 * (1, 2, 3, ...) ela vira uma lista e a altura é n. O teste mostra os dois casos. */

typedef struct NoArvore {
    int valor;
    struct NoArvore *esquerda;
    struct NoArvore *direita;
} NoArvore;

typedef struct {
    NoArvore *raiz;
    size_t tamanho;
} Arvore;

Arvore *arvore_criar(void);
void arvore_destruir(Arvore *a);

bool arvore_inserir(Arvore *a, int valor);   /* false se já existia (sem repetidos) */
bool arvore_contem(const Arvore *a, int valor);
bool arvore_remover(Arvore *a, int valor);
bool arvore_minimo(const Arvore *a, int *valor);
bool arvore_maximo(const Arvore *a, int *valor);
int arvore_altura(const Arvore *a);          /* árvore vazia = 0, só a raiz = 1 */

/* Percorre em ordem (esquerda, nó, direita): sai tudo ordenado. Devolve quantos escreveu. */
size_t arvore_em_ordem(const Arvore *a, int *saida, size_t max);

#endif
