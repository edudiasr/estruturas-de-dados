#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stddef.h>

/* Lista duplamente ligada de inteiros.
 *
 *   NULL <- [3] <-> [7] <-> [1] -> NULL
 *           inicio          fim
 *
 * Guardar ponteiro pro fim deixa inserir no fim em O(1); sem ele seria O(n) (andar até o último). */

typedef struct No {
    int valor;
    struct No *anterior;
    struct No *proximo;
} No;

typedef struct {
    No *inicio;
    No *fim;
    size_t tamanho;
} Lista;

Lista *lista_criar(void);
void lista_destruir(Lista *l);

bool lista_inserir_inicio(Lista *l, int valor);   /* O(1) */
bool lista_inserir_fim(Lista *l, int valor);      /* O(1) */
bool lista_inserir_em(Lista *l, size_t pos, int valor); /* O(n) */

bool lista_remover_inicio(Lista *l, int *valor);  /* O(1) */
bool lista_remover_fim(Lista *l, int *valor);     /* O(1): é aqui que a ligação dupla ajuda */
bool lista_remover_valor(Lista *l, int valor);    /* O(n): remove a primeira ocorrência */

bool lista_obter(const Lista *l, size_t pos, int *valor); /* O(n) */
long lista_buscar(const Lista *l, int valor);     /* O(n): posição ou -1 */
void lista_inverter(Lista *l);                    /* O(n), sem alocar nada */

#endif
