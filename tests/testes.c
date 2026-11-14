/* Testes sem biblioteca: cada VERIFICA que falha mostra arquivo e linha, e no fim o programa
 * sai com código 1 se algo falhou. No CI eles rodam com AddressSanitizer e Valgrind, que acusam
 * vazamento de memória e acesso fora do lugar. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/arvore.h"
#include "../src/grafo.h"
#include "../src/hash.h"
#include "../src/heap.h"
#include "../src/lista.h"
#include "../src/pilha_fila.h"

static int verificacoes = 0, falhas = 0;

#define VERIFICA(cond)                                                     \
    do {                                                                   \
        verificacoes++;                                                    \
        if (!(cond)) {                                                     \
            falhas++;                                                      \
            fprintf(stderr, "%s:%d  falhou: %s\n", __FILE__, __LINE__, #cond); \
        }                                                                  \
    } while (0)

/* confere o conteúdo da lista nos dois sentidos: pega erro de ponteiro "anterior" esquecido */
static int lista_igual(const Lista *l, const int *esperado, size_t n) {
    if (l->tamanho != n) return 0;
    size_t i = 0;
    for (No *no = l->inicio; no; no = no->proximo, i++) if (no->valor != esperado[i]) return 0;
    i = n;
    for (No *no = l->fim; no; no = no->anterior) if (no->valor != esperado[--i]) return 0;
    return 1;
}

static void testa_lista(void) {
    Lista *l = lista_criar();
    int v;
    VERIFICA(!lista_remover_inicio(l, &v));            /* vazia */
    lista_inserir_fim(l, 2);
    lista_inserir_inicio(l, 1);
    lista_inserir_fim(l, 4);
    lista_inserir_em(l, 2, 3);                          /* no meio */
    VERIFICA(lista_igual(l, (int[]){1, 2, 3, 4}, 4));
    VERIFICA(!lista_inserir_em(l, 9, 0));               /* posição inválida */

    VERIFICA(lista_buscar(l, 3) == 2);
    VERIFICA(lista_buscar(l, 99) == -1);
    VERIFICA(lista_obter(l, 3, &v) && v == 4);

    lista_inverter(l);
    VERIFICA(lista_igual(l, (int[]){4, 3, 2, 1}, 4));

    VERIFICA(lista_remover_valor(l, 3));
    VERIFICA(lista_remover_fim(l, &v) && v == 1);
    VERIFICA(lista_remover_inicio(l, &v) && v == 4);
    VERIFICA(lista_igual(l, (int[]){2}, 1));
    VERIFICA(lista_remover_fim(l, &v) && v == 2);
    VERIFICA(l->inicio == NULL && l->fim == NULL);      /* esvaziou direito */
    lista_destruir(l);
}

static void testa_pilha_e_fila(void) {
    Pilha *p = pilha_criar();
    int v;
    for (int i = 0; i < 1000; i++) pilha_empilhar(p, i); /* passa por várias realocações */
    VERIFICA(pilha_topo(p, &v) && v == 999);
    for (int i = 999; i >= 0; i--) VERIFICA(pilha_desempilhar(p, &v) && v == i);
    VERIFICA(!pilha_desempilhar(p, &v));
    pilha_destruir(p);

    Fila *f = fila_criar();
    /* enche, tira metade e enche de novo: força o "dar a volta" no vetor e o crescimento com a volta dada */
    for (int i = 0; i < 4; i++) fila_enfileirar(f, i);
    for (int i = 0; i < 3; i++) fila_desenfileirar(f, &v);
    for (int i = 4; i < 20; i++) fila_enfileirar(f, i);
    for (int i = 3; i < 20; i++) VERIFICA(fila_desenfileirar(f, &v) && v == i);
    VERIFICA(!fila_frente(f, &v));
    fila_destruir(f);

    VERIFICA(expressao_balanceada("{[(a + b) * c] - (d / e)}"));
    VERIFICA(expressao_balanceada("sem parênteses"));
    VERIFICA(!expressao_balanceada("(]"));
    VERIFICA(!expressao_balanceada("(()"));
    VERIFICA(!expressao_balanceada(")("));
}

static void testa_arvore(void) {
    Arvore *a = arvore_criar();
    int entrada[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
    for (size_t i = 0; i < 9; i++) VERIFICA(arvore_inserir(a, entrada[i]));
    VERIFICA(!arvore_inserir(a, 6)); /* repetido */
    VERIFICA(a->tamanho == 9);
    VERIFICA(arvore_altura(a) == 4);

    int ordem[16];
    VERIFICA(arvore_em_ordem(a, ordem, 16) == 9);
    VERIFICA(memcmp(ordem, (int[]){1, 3, 4, 6, 7, 8, 10, 13, 14}, 9 * sizeof(int)) == 0);

    int v;
    VERIFICA(arvore_minimo(a, &v) && v == 1);
    VERIFICA(arvore_maximo(a, &v) && v == 14);

    VERIFICA(arvore_remover(a, 1));   /* folha */
    VERIFICA(arvore_remover(a, 14));  /* um filho */
    VERIFICA(arvore_remover(a, 3));   /* dois filhos */
    VERIFICA(arvore_remover(a, 8));   /* a raiz, com dois filhos */
    VERIFICA(!arvore_remover(a, 99));
    VERIFICA(arvore_em_ordem(a, ordem, 16) == 5);
    VERIFICA(memcmp(ordem, (int[]){4, 6, 7, 10, 13}, 5 * sizeof(int)) == 0);
    VERIFICA(!arvore_contem(a, 8) && arvore_contem(a, 7));
    arvore_destruir(a);

    /* o pior caso: inserir já ordenado vira uma "lista" de altura n */
    Arvore *torta = arvore_criar();
    for (int i = 1; i <= 100; i++) arvore_inserir(torta, i);
    VERIFICA(arvore_altura(torta) == 100);
    arvore_destruir(torta);
}

static int compara(const void *a, const void *b) {
    return (*(const int *)a > *(const int *)b) - (*(const int *)a < *(const int *)b);
}

static void testa_heap(void) {
    Heap *h = heap_criar();
    int entrada[] = {5, 3, 8, 1, 9, 2, 7, 1};
    for (size_t i = 0; i < 8; i++) heap_inserir(h, entrada[i]);
    int saida[8], v;
    for (size_t i = 0; i < 8; i++) VERIFICA(heap_extrair_minimo(h, &saida[i]));
    VERIFICA(memcmp(saida, (int[]){1, 1, 2, 3, 5, 7, 8, 9}, sizeof saida) == 0);
    VERIFICA(!heap_minimo(h, &v));
    heap_destruir(h);

    /* heapsort contra o qsort da biblioteca, com 10 mil números aleatórios */
    enum { N = 10000 };
    int *a = malloc(N * sizeof(int)), *b = malloc(N * sizeof(int));
    srand(42);
    for (int i = 0; i < N; i++) a[i] = b[i] = rand() % 1000 - 500;
    heapsort(a, N);
    qsort(b, N, sizeof(int), compara);
    VERIFICA(memcmp(a, b, N * sizeof(int)) == 0);
    free(a);
    free(b);
    heapsort(NULL, 0); /* vazio não quebra */
}

static void testa_hash(void) {
    Hash *h = hash_criar();
    VERIFICA(hash_definir(h, "maçã", 3));
    VERIFICA(hash_definir(h, "uva", 7));
    VERIFICA(hash_definir(h, "maçã", 5)); /* substitui */
    int v;
    VERIFICA(hash_obter(h, "maçã", &v) && v == 5);
    VERIFICA(!hash_obter(h, "pera", &v));
    VERIFICA(h->tamanho == 2);

    /* 1000 chaves: obriga a tabela a crescer várias vezes, e tudo continua achável */
    char chave[32];
    for (int i = 0; i < 1000; i++) {
        snprintf(chave, sizeof chave, "jogador-%d", i);
        hash_definir(h, chave, i);
    }
    VERIFICA(h->tamanho == 1002);
    VERIFICA(h->tamanho * 4 <= h->num_baldes * 3); /* carga abaixo de 75% */
    int todos = 1;
    for (int i = 0; i < 1000; i++) {
        snprintf(chave, sizeof chave, "jogador-%d", i);
        todos &= hash_obter(h, chave, &v) && v == i;
    }
    VERIFICA(todos);

    VERIFICA(hash_remover(h, "uva"));
    VERIFICA(!hash_remover(h, "uva"));
    VERIFICA(!hash_obter(h, "uva", &v));
    VERIFICA(hash_djb2("a") == 5381 * 33 + 'a');
    hash_destruir(h);
}

/* cidades de SC e estradas entre elas (km aproximados, só pro exemplo) */
enum { JOINVILLE, BLUMENAU, ITAJAI, FLORIANOPOLIS, LAGES, CHAPECO, CIDADES };
static const char *NOMES[] = {"Joinville", "Blumenau", "Itajaí", "Florianópolis", "Lages", "Chapecó"};

static void testa_grafo(void) {
    Grafo *g = grafo_criar(CIDADES);
    grafo_ligar(g, JOINVILLE, BLUMENAU, 95);
    grafo_ligar(g, JOINVILLE, ITAJAI, 90);
    grafo_ligar(g, BLUMENAU, ITAJAI, 55);
    grafo_ligar(g, ITAJAI, FLORIANOPOLIS, 95);
    grafo_ligar(g, BLUMENAU, LAGES, 230);
    grafo_ligar(g, FLORIANOPOLIS, LAGES, 225);
    VERIFICA(!grafo_ligar(g, JOINVILLE, 99, 1));
    VERIFICA(!grafo_ligar(g, JOINVILLE, LAGES, -5)); /* Dijkstra não aceita peso negativo */

    long saltos[CIDADES];
    VERIFICA(grafo_bfs(g, JOINVILLE, saltos));
    VERIFICA(saltos[JOINVILLE] == 0 && saltos[ITAJAI] == 1 && saltos[LAGES] == 2);
    VERIFICA(saltos[CHAPECO] == GRAFO_SEM_CAMINHO); /* sem estrada no exemplo */

    long distancia[CIDADES], anterior[CIDADES];
    VERIFICA(grafo_dijkstra(g, JOINVILLE, distancia, anterior));
    VERIFICA(distancia[FLORIANOPOLIS] == 185);
    /* pra Lages, ir por Blumenau (95 + 230 = 325) ganha de ir pelo litoral (90 + 95 + 225 = 410) */
    VERIFICA(distancia[LAGES] == 325);
    size_t rota[CIDADES];
    size_t n = grafo_caminho(anterior, LAGES, rota, CIDADES);
    VERIFICA(n == 3 && rota[0] == JOINVILLE && rota[1] == BLUMENAU && rota[2] == LAGES);
    VERIFICA(distancia[CHAPECO] == GRAFO_SEM_CAMINHO);

    printf("  rota Joinville -> Lages (%ld km):", distancia[LAGES]);
    for (size_t i = 0; i < n; i++) printf(" %s", NOMES[rota[i]]);
    printf("\n");
    grafo_destruir(g);
}

int main(void) {
    testa_lista();
    testa_pilha_e_fila();
    testa_arvore();
    testa_heap();
    testa_hash();
    testa_grafo();
    printf("%d/%d verificações ok\n", verificacoes - falhas, verificacoes);
    return falhas ? 1 : 0;
}
