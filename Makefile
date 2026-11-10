# Makefile do Chuva de Frutas - baseado no Makefile da Aula 18 do professor
# Funciona no Windows (w64devkit), no WSL/Linux e no macOS
# Uso:  make        -> compila o jogo
#       make clean  -> apaga o executavel

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99
FONTES  = $(wildcard *.c)
HEADERS = $(wildcard *.h)
ALVO    = jogo

ifeq ($(OS),Windows_NT)
    # Windows nativo: raylib instalada pelo instalador oficial em C:\raylib
    ALVO    = jogo.exe
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

clean:
	rm -f jogo jogo.exe *.o

.PHONY: all clean
