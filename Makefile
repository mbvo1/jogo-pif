# Makefile do Chuva de Frutas - baseado no Makefile da Aula 18 do professor
# Funciona no Windows (w64devkit), no WSL/Linux e no macOS
# Uso:  make        -> compila o jogo
#       make teste  -> compila e roda os testes das funcoes puras
#       make clean  -> apaga os executaveis

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99
FONTES  = $(wildcard *.c)
HEADERS = $(wildcard *.h)
ALVO    = jogo
TESTE   = testes/teste_cesta

ifeq ($(OS),Windows_NT)
    # Windows nativo: raylib instalada pelo instalador oficial em C:\raylib
    ALVO    = jogo.exe
    TESTE   = testes/teste_cesta.exe
    RAYLIB ?= C:/raylib/raylib/src
    CFLAGS += -I$(RAYLIB)
    LDLIBS  = -L$(RAYLIB) -lraylib -lopengl32 -lgdi32 -lwinmm
else ifeq ($(shell uname -s),Darwin)
    # macOS: brew install raylib
    CFLAGS += $(shell pkg-config --cflags raylib)
    LDLIBS  = $(shell pkg-config --libs raylib)
else
    # Linux / WSL: raylib compilada e instalada com sudo make install
    LDLIBS  = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

all: $(ALVO)

$(ALVO): $(FONTES) $(HEADERS)
	$(CC) $(CFLAGS) $(FONTES) -o $(ALVO) $(LDLIBS)

# Os testes usam so cesta.c (sem o main.c do jogo)
teste: $(TESTE)
	./$(TESTE)

$(TESTE): testes/teste_cesta.c cesta.c cesta.h
	$(CC) $(CFLAGS) testes/teste_cesta.c cesta.c -o $(TESTE) $(LDLIBS)

clean:
	rm -f jogo jogo.exe $(TESTE) *.o

.PHONY: all teste clean
