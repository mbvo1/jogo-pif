#ifndef CESTA_H
#define CESTA_H

#include "raylib.h"

typedef struct Cesta {
    Rectangle corpo;   /* posicao e tamanho da cesta na tela */
    float velocidade;  /* em pixels por SEGUNDO */
} Cesta;

/* Funcoes puras: so fazem calculo */
float moveX(float x, float velocidade, int direcao, float dt);
float limitaX(float x, float largura, float larguraTela);

/* Atualizacao (impura): le o teclado e muda a cesta */
void atualizaCesta(Cesta *c, float dt, int larguraTela);

/* Desenho: mostra a cesta e nao altera o estado do jogo */
void desenhaCesta(Cesta c);

#endif
