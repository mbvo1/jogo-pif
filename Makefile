# Makefile do Chuva de Frutas - baseado no Makefile da Aula 18 do professor
# Funciona no Windows (w64devkit), no WSL/Linux e no macOS
# Uso:  make        -> compila o jogo
#       make teste  -> compila e roda TODOS os testes (sem abrir janela)
#       make clean  -> apaga os executaveis

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99
FONTES  = $(wildcard *.c)
HEADERS = $(wildcard *.h)
# Os testes tem o proprio main, entao usam todos os .c MENOS o main.c
MODULOS = $(filter-out main.c, $(FONTES))
ALVO    = jogo
EXT     =

ifeq ($(OS),Windows_NT)
    # Windows nativo: raylib instalada pelo instalador oficial em C:\raylib
    ALVO    = jogo.exe
    EXT     = .exe
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

TESTES = testes/teste_cesta$(EXT) testes/teste_itens$(EXT) testes/teste_jogo$(EXT)

all: $(ALVO)

$(ALVO): $(FONTES) $(HEADERS)
	$(CC) $(CFLAGS) $(FONTES) -o $(ALVO) $(LDLIBS)

# Roda um teste depois do outro; se algum falhar, o make para
teste: $(TESTES)
	./testes/teste_cesta$(EXT)
	./testes/teste_itens$(EXT)
	./testes/teste_jogo$(EXT)

# Cada teste = seu arquivo testes/teste_X.c + os modulos do jogo (sem main.c)
testes/teste_%$(EXT): testes/teste_%.c $(MODULOS) $(HEADERS)
	$(CC) $(CFLAGS) $< $(MODULOS) -o $@ $(LDLIBS)

clean:
	rm -f jogo jogo.exe $(TESTES) *.o

.PHONY: all teste clean
