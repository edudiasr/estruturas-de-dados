#include "hash.h"

#include <stdlib.h>
#include <string.h>

#define BALDES_INICIAIS 8

unsigned long hash_djb2(const char *texto) {
    unsigned long h = 5381;
    for (const unsigned char *c = (const unsigned char *)texto; *c; c++) {
        h = h * 33 + *c;
    }
    return h;
}

Hash *hash_criar(void) {
    Hash *h = malloc(sizeof(Hash));
    if (!h) return NULL;
    h->baldes = calloc(BALDES_INICIAIS, sizeof(Entrada *));
    if (!h->baldes) {
        free(h);
        return NULL;
    }
    h->num_baldes = BALDES_INICIAIS;
    h->tamanho = 0;
    return h;
}

void hash_destruir(Hash *h) {
    if (!h) return;
    for (size_t i = 0; i < h->num_baldes; i++) {
        Entrada *e = h->baldes[i];
        while (e) {
            Entrada *proxima = e->proxima;
            free(e->chave);
            free(e);
            e = proxima;
        }
    }
    free(h->baldes);
    free(h);
}

static bool redimensionar(Hash *h) {
    size_t novos = h->num_baldes * 2;
    Entrada **baldes = calloc(novos, sizeof(Entrada *));
    if (!baldes) return false;
    /* reaproveita as entradas: só muda de balde, nada é copiado nem alocado */
    for (size_t i = 0; i < h->num_baldes; i++) {
        Entrada *e = h->baldes[i];
        while (e) {
            Entrada *proxima = e->proxima;
            size_t destino = hash_djb2(e->chave) % novos;
            e->proxima = baldes[destino];
            baldes[destino] = e;
            e = proxima;
        }
    }
    free(h->baldes);
    h->baldes = baldes;
    h->num_baldes = novos;
    return true;
}

static char *copiar(const char *texto) {
    size_t n = strlen(texto) + 1;
    char *c = malloc(n);
    if (c) memcpy(c, texto, n);
    return c;
}

bool hash_definir(Hash *h, const char *chave, int valor) {
    size_t i = hash_djb2(chave) % h->num_baldes;
    for (Entrada *e = h->baldes[i]; e; e = e->proxima) {
        if (strcmp(e->chave, chave) == 0) {
            e->valor = valor; /* já existia: só troca o valor */
            return true;
        }
    }
    /* 75%: limite comum entre memória gasta e tamanho médio das listas */
    if ((h->tamanho + 1) * 4 > h->num_baldes * 3) {
        if (!redimensionar(h)) return false;
        i = hash_djb2(chave) % h->num_baldes;
    }
    Entrada *nova = malloc(sizeof(Entrada));
    if (!nova) return false;
    nova->chave = copiar(chave); /* guarda uma cópia: quem chamou pode liberar a string dele */
    if (!nova->chave) {
        free(nova);
        return false;
    }
    nova->valor = valor;
    nova->proxima = h->baldes[i];
    h->baldes[i] = nova;
    h->tamanho++;
    return true;
}

bool hash_obter(const Hash *h, const char *chave, int *valor) {
    for (Entrada *e = h->baldes[hash_djb2(chave) % h->num_baldes]; e; e = e->proxima) {
        if (strcmp(e->chave, chave) == 0) {
            *valor = e->valor;
            return true;
        }
    }
    return false;
}

bool hash_remover(Hash *h, const char *chave) {
    Entrada **lugar = &h->baldes[hash_djb2(chave) % h->num_baldes];
    while (*lugar) {
        if (strcmp((*lugar)->chave, chave) == 0) {
            Entrada *remover = *lugar;
            *lugar = remover->proxima;
            free(remover->chave);
            free(remover);
            h->tamanho--;
            return true;
        }
        lugar = &(*lugar)->proxima;
    }
    return false;
}
