#ifndef JOGO_H
#define JOGO_H

#include "raylib.h"
#include "cesta.h"
#include "item.h"

#define LARGURA 800
#define ALTURA 450

#define VIDAS_INICIAIS 3
#define PONTOS_PARA_VENCER 30
#define TEMPO_DE_JOGO 60.0f    /* segundos de partida */

#define INTERVALO_ITENS 0.8f   /* segundos entre um item e o proximo */

typedef enum { TELA_INICIO, TELA_JOGANDO, TELA_VITORIA, TELA_DERROTA } Tela;

/* Todo o estado do jogo fica junto nesta struct */
typedef struct Jogo {
    Cesta cesta;
    Item itens[MAX_ITENS];
    int numItens;            /* quantos itens do vetor estao em uso */
    int pontos;
    int vidas;
    float tempoRestante;     /* em segundos, conta para baixo */
    float tempoProximoItem;  /* contagem regressiva ate o proximo item */
    Tela tela;               /* qual tela esta aparecendo */
} Jogo;

/* Funcoes puras: as regras de tempo e de fim de jogo */
float descontaTempo(float tempoRestante, float dt);
Tela verificaResultado(int pontos, int vidas, float tempoRestante);

/* Zera tudo para comecar uma partida (no inicio e no ENTER) */
void iniciaJogo(Jogo *j);

/* Atualizacao (impura): le teclado, sorteia itens e muda o estado */
void atualizaJogo(Jogo *j, float dt);

/* Desenho: mostra o estado e NAO altera nada (recebe por valor) */
void desenhaJogo(Jogo j);

#endif
