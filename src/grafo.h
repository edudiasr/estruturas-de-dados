#ifndef GRAFO_H
#define GRAFO_H

#include <stdbool.h>
#include <stddef.h>

/* Grafo com peso nas arestas, guardado como lista de adjacência (cada vértice tem a lista dos
 * vizinhos). Ocupa O(V + E), bom pra grafos com poucas arestas, como mapas de ruas.
 *
 * Exemplo do teste: cidades de SC ligadas por estrada, com a distância em km no peso. */

#define GRAFO_SEM_CAMINHO (-1L)

typedef struct Aresta {
    size_t destino;
    long peso;
    struct Aresta *proxima;
} Aresta;

typedef struct {
    Aresta **adjacentes;
    size_t num_vertices;
} Grafo;

Grafo *grafo_criar(size_t num_vertices);
void grafo_destruir(Grafo *g);
bool grafo_ligar(Grafo *g, size_t a, size_t b, long peso); /* aresta nos dois sentidos */

/* Busca em largura: quantas arestas no mínimo de `origem` até cada vértice (ignora o peso).
 * Preenche saltos[v]; vértice inalcançável fica com GRAFO_SEM_CAMINHO. O(V + E). */
bool grafo_bfs(const Grafo *g, size_t origem, long *saltos);

/* Dijkstra: menor distância somando os pesos (que não podem ser negativos).
 * anterior[v] guarda de onde se chega em v no melhor caminho, pra remontar a rota.
 * O(V^2) nesta versão simples, que procura o próximo vértice varrendo o vetor. */
bool grafo_dijkstra(const Grafo *g, size_t origem, long *distancia, long *anterior);

/* Remonta o caminho até `destino` a partir do vetor `anterior`. Devolve quantos vértices escreveu. */
size_t grafo_caminho(const long *anterior, size_t destino, size_t *saida, size_t max);

#endif
