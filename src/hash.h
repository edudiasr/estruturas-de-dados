#ifndef HASH_H
#define HASH_H

#include <stdbool.h>
#include <stddef.h>

/* Tabela hash de texto -> inteiro, com encadeamento: cada posição do vetor ("balde") guarda
 * uma lista ligada das chaves que caíram ali.
 *
 *   balde 0: -
 *   balde 1: "maçã"=3 -> "uva"=7
 *   balde 2: "pera"=1
 *
 * Buscar é O(1) em média: calcula o balde e percorre uma lista curta. Pra lista continuar curta,
 * quando o número de chaves passa de 75% do número de baldes a tabela dobra e redistribui tudo. */

typedef struct Entrada {
    char *chave;
    int valor;
    struct Entrada *proxima;
} Entrada;

typedef struct {
    Entrada **baldes;
    size_t num_baldes;
    size_t tamanho;
} Hash;

Hash *hash_criar(void);
void hash_destruir(Hash *h);

bool hash_definir(Hash *h, const char *chave, int valor); /* insere ou substitui */
bool hash_obter(const Hash *h, const char *chave, int *valor);
bool hash_remover(Hash *h, const char *chave);

/* função de espalhamento djb2 (Dan Bernstein): simples e espalha bem texto comum */
unsigned long hash_djb2(const char *texto);

#endif
