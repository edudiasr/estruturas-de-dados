#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>
#include <stddef.h>

/* Heap de mínimo guardado num vetor: o menor valor fica sempre na posição 0.
 * Os filhos da posição i ficam em 2i+1 e 2i+2, e o pai em (i-1)/2: não precisa de ponteiro.
 *
 *   vetor: [1, 3, 2, 7, 4]          árvore:      1
 *                                              /   \
 *                                             3     2
 *                                            / \
 *                                           7   4
 *
 * Inserir e tirar o menor custam O(log n): o valor sobe ou desce uma "altura" da árvore. */

typedef struct {
    int *dados;
    size_t tamanho;
    size_t capacidade;
} Heap;

Heap *heap_criar(void);
void heap_destruir(Heap *h);
bool heap_inserir(Heap *h, int valor);
bool heap_extrair_minimo(Heap *h, int *valor);
bool heap_minimo(const Heap *h, int *valor);

/* Ordena o vetor no lugar em O(n log n), sem memória extra (heapsort). */
void heapsort(int *v, size_t n);

#endif
