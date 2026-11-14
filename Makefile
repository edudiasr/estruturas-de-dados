CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SRC = src/lista.c src/pilha_fila.c src/arvore.c src/heap.c src/hash.c src/grafo.c

.PHONY: test asan valgrind comparacao clean

build/testes: tests/testes.c $(SRC) src/*.h
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ tests/testes.c $(SRC)

build/comparacao: tests/comparacao.c $(SRC) src/*.h
	@mkdir -p build
	$(CC) $(CFLAGS) -D_POSIX_C_SOURCE=199309L -o $@ tests/comparacao.c $(SRC)

test: build/testes
	./build/testes

# AddressSanitizer + UndefinedBehaviorSanitizer: acusam acesso fora do vetor, uso depois do free,
# vazamento e comportamento indefinido (ex.: estouro de inteiro com sinal)
asan:
	@mkdir -p build
	$(CC) -std=c11 -g -O1 -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer \
		-o build/testes-asan tests/testes.c $(SRC)
	./build/testes-asan

valgrind: build/testes
	valgrind --leak-check=full --error-exitcode=1 ./build/testes

comparacao: build/comparacao
	./build/comparacao

clean:
	rm -rf build
