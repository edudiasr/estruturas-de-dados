# estruturas-de-dados

Estruturas de dados clássicas escritas do zero em C, sem biblioteca nenhuma além da padrão. Comecei
como exercício da disciplina e fui completando: cada uma tem teste, e o CI roda tudo com
AddressSanitizer, UBSan e Valgrind, pra garantir que não tem vazamento de memória nem acesso fora
do lugar.

| Estrutura | Arquivo | O que tem |
|---|---|---|
| Lista duplamente ligada | `src/lista.c` | inserir/remover no início, no fim e numa posição; buscar; inverter sem alocar |
| Pilha | `src/pilha_fila.c` | vetor que dobra de tamanho quando enche |
| Fila | `src/pilha_fila.c` | vetor circular (o começo "dá a volta"), também cresce |
| Árvore binária de busca | `src/arvore.c` | inserir, buscar, remover (inclusive nó com dois filhos), mínimo, máximo, altura, percurso em ordem |
| Heap mínimo + heapsort | `src/heap.c` | fila de prioridade e ordenação in-place |
| Tabela hash | `src/hash.c` | chave texto, função djb2, colisão por encadeamento, cresce quando passa de 75% de ocupação |
| Grafo | `src/grafo.c` | lista de adjacência, busca em largura (BFS), Dijkstra e reconstrução do caminho |

Também tem um uso prático de pilha (`expressao_balanceada`, que confere se `([{ }])` estão fechados
na ordem certa) e o grafo é testado com algumas cidades de SC: o Dijkstra acha que de Joinville pra
Lages compensa ir por Blumenau (325 km) em vez de descer pelo litoral (410 km).

## Rodando

Precisa de `gcc` (ou `clang`) e `make`. No Windows funciona pelo WSL.

```bash
make test         # compila e roda os testes
make asan         # mesmos testes com AddressSanitizer + UndefinedBehaviorSanitizer
make valgrind     # mesmos testes dentro do Valgrind
make comparacao   # mede o tempo de busca em lista, árvore e hash
```

Os testes não usam framework: é uma macro `VERIFICA(condição)` que conta as verificações, mostra
arquivo e linha do que falhou e faz o programa sair com erro. Hoje são 1097 verificações.

## Complexidade

| Operação | Lista ligada | Pilha / Fila | Árvore de busca | Heap | Hash |
|---|---|---|---|---|---|
| Inserir | O(1) nas pontas, O(n) no meio | O(1) amortizado | O(log n) médio, O(n) pior | O(log n) | O(1) médio |
| Remover | O(1) nas pontas, O(n) por valor | O(1) | O(log n) médio, O(n) pior | O(log n) (o mínimo) | O(1) médio |
| Buscar | O(n) | — | O(log n) médio, O(n) pior | O(n) | O(1) médio |
| Menor elemento | O(n) | — | O(log n) médio | O(1) | O(n) |

- Grafo: BFS é O(V + E). O Dijkstra aqui é a versão simples, O(V²), que escolhe o próximo vértice
  varrendo o vetor de distâncias. Com o heap daria O((V + E) log V), mas pra grafos pequenos a
  simples é mais fácil de conferir.
- Heapsort: O(n log n) sempre, sem memória extra.
- "Amortizado" na pilha/fila: quando o vetor enche ele dobra (custa O(n) naquela vez), mas como isso
  acontece cada vez mais raramente, a média por operação continua O(1).

### O "pior caso" da árvore

A árvore de busca não se balanceia sozinha. Se os números chegam em ordem (1, 2, 3, ...), cada um
vira filho direito do anterior e a árvore vira uma lista: altura 100 para 100 elementos, e a busca
volta a ser O(n). Tem um teste que mostra isso. Árvores AVL e rubro-negra resolvem esse problema
girando os nós; é o próximo passo que quero implementar.

## Medindo na prática

`make comparacao` coloca de 1 mil a 100 mil números embaralhados em cada estrutura e faz 2000
buscas aleatórias. Resultado na minha máquina (tempo médio por busca):

| n | Lista | Árvore | Hash | Altura da árvore |
|---:|---:|---:|---:|---:|
| 1.000 | 1,11 µs | 0,05 µs | 0,08 µs | 24 |
| 10.000 | 30,64 µs | 0,12 µs | 0,18 µs | 31 |
| 100.000 | 1651,37 µs | 5,73 µs | 0,33 µs | 41 |

São números de uma execução só, então variam um pouco de uma vez pra outra (rodando de novo, a
árvore com 100 mil já deu entre 1 e 6 µs). A ordem de grandeza é o que importa:

- A lista cresce junto com n, e no último fica pior que linear porque 100 mil nós espalhados pela
  memória não cabem no cache do processador.
- A árvore cresce devagar: com números embaralhados a altura fica perto de 2,5·log₂(n), bem longe
  do pior caso. Com 100 mil ela também sofre com cache, já que cada nó é uma alocação separada.
- O hash quase não muda. Com poucos elementos ele perde da árvore porque cada busca transforma o
  número em texto com `snprintf` e calcula o djb2, o que custa mais que comparar alguns inteiros.

## Organização

```
src/       uma estrutura por par .h/.c
tests/     testes.c (as verificações) e comparacao.c (medição de tempo)
Makefile
```

## Licença

MIT
