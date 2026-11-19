#ifndef JOGO_H
#define JOGO_H

#include "raylib.h"
#include "cesta.h"
#include "item.h"

#define LARGURA 800
#define ALTURA 450

#define VIDAS_INICIAIS 3
#define PONTOS_PARA_VENCER 30

#define INTERVALO_ITENS 0.8f   /* segundos entre um item e o proximo */

/* Todo o estado do jogo fica junto nesta struct */
typedef struct Jogo {
    Cesta cesta;
    Item itens[MAX_ITENS];
    int numItens;            /* quantos itens do vetor estao em uso */
    int pontos;
    int vidas;
    float tempoProximoItem;  /* contagem regressiva ate o proximo item */
} Jogo;

/* Zera tudo para comecar uma partida */
void iniciaJogo(Jogo *j);

/* Atualizacao (impura): le teclado, sorteia itens e muda o estado */
void atualizaJogo(Jogo *j, float dt);

/* Desenho: mostra o estado e NAO altera nada (recebe por valor) */
void desenhaJogo(Jogo j);

#endif
