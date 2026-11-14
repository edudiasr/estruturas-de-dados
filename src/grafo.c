#include "grafo.h"

#include <stdlib.h>

#include "pilha_fila.h"

Grafo *grafo_criar(size_t num_vertices) {
    Grafo *g = malloc(sizeof(Grafo));
    if (!g) return NULL;
    g->adjacentes = calloc(num_vertices, sizeof(Aresta *));
    if (!g->adjacentes) {
        free(g);
        return NULL;
    }
    g->num_vertices = num_vertices;
    return g;
}

void grafo_destruir(Grafo *g) {
    if (!g) return;
    for (size_t v = 0; v < g->num_vertices; v++) {
        Aresta *a = g->adjacentes[v];
        while (a) {
            Aresta *proxima = a->proxima;
            free(a);
            a = proxima;
        }
    }
    free(g->adjacentes);
    free(g);
}

static bool adicionar(Grafo *g, size_t de, size_t para, long peso) {
    Aresta *a = malloc(sizeof(Aresta));
    if (!a) return false;
    a->destino = para;
    a->peso = peso;
    a->proxima = g->adjacentes[de];
    g->adjacentes[de] = a;
    return true;
}

bool grafo_ligar(Grafo *g, size_t a, size_t b, long peso) {
    if (a >= g->num_vertices || b >= g->num_vertices || peso < 0) return false;
    return adicionar(g, a, b, peso) && adicionar(g, b, a, peso);
}

bool grafo_bfs(const Grafo *g, size_t origem, long *saltos) {
    if (origem >= g->num_vertices) return false;
    for (size_t v = 0; v < g->num_vertices; v++) saltos[v] = GRAFO_SEM_CAMINHO;

    Fila *fila = fila_criar(); /* reaproveita a fila do pilha_fila.c */
    if (!fila) return false;
    saltos[origem] = 0;
    bool ok = fila_enfileirar(fila, (int)origem);
    int atual;
    while (ok && fila_desenfileirar(fila, &atual)) {
        for (Aresta *a = g->adjacentes[atual]; a; a = a->proxima) {
            if (saltos[a->destino] == GRAFO_SEM_CAMINHO) { /* primeira vez que chega: é o caminho mais curto */
                saltos[a->destino] = saltos[atual] + 1;
                ok = fila_enfileirar(fila, (int)a->destino);
            }
        }
    }
    fila_destruir(fila);
    return ok;
}

bool grafo_dijkstra(const Grafo *g, size_t origem, long *distancia, long *anterior) {
    size_t n = g->num_vertices;
    if (origem >= n) return false;
    bool *fechado = calloc(n, sizeof(bool));
    if (!fechado) return false;
    for (size_t v = 0; v < n; v++) {
        distancia[v] = GRAFO_SEM_CAMINHO;
        anterior[v] = GRAFO_SEM_CAMINHO;
    }
    distancia[origem] = 0;

    for (;;) {
        /* o vértice aberto com menor distância conhecida já tem a distância definitiva */
        long melhor = GRAFO_SEM_CAMINHO;
        size_t u = 0;
        for (size_t v = 0; v < n; v++) {
            if (!fechado[v] && distancia[v] != GRAFO_SEM_CAMINHO && (melhor == GRAFO_SEM_CAMINHO || distancia[v] < melhor)) {
                melhor = distancia[v];
                u = v;
            }
        }
        if (melhor == GRAFO_SEM_CAMINHO) break; /* o resto é inalcançável */
        fechado[u] = true;

        for (Aresta *a = g->adjacentes[u]; a; a = a->proxima) {
            long candidata = distancia[u] + a->peso;
            if (!fechado[a->destino] && (distancia[a->destino] == GRAFO_SEM_CAMINHO || candidata < distancia[a->destino])) {
                distancia[a->destino] = candidata;
                anterior[a->destino] = (long)u;
            }
        }
    }
    free(fechado);
    return true;
}

size_t grafo_caminho(const long *anterior, size_t destino, size_t *saida, size_t max) {
    /* anda de trás pra frente e depois inverte */
    size_t n = 0;
    for (long v = (long)destino; v != GRAFO_SEM_CAMINHO && n < max; v = anterior[v]) {
        saida[n++] = (size_t)v;
    }
    for (size_t i = 0; i < n / 2; i++) {
        size_t t = saida[i];
        saida[i] = saida[n - 1 - i];
        saida[n - 1 - i] = t;
    }
    return n;
}
