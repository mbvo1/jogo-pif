#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include "raylib.h"

#define MAX_ITENS 32           /* tamanho maximo do vetor de itens */
#define RAIO_ITEM 14.0f        /* em pixels */
#define VELOCIDADE_QUEDA 200.0f /* em pixels por SEGUNDO */

#define VALOR_MACA 1
#define VALOR_LARANJA 2
#define VALOR_BOMBA 0

typedef enum { MACA, LARANJA, BOMBA } TipoItem;

typedef struct Item {
    Vector2 pos;       /* centro do item na tela */
    float raio;
    float velocidade;  /* em pixels por SEGUNDO, para baixo */
    TipoItem tipo;
    int valor;         /* pontos que o item vale se for pego */
} Item;

/* Funcoes puras: recebem por valor e so fazem calculo */
Item criaItem(float x, TipoItem tipo);
Item caiItem(Item it, float dt);

/* Funcoes sobre a colecao (vetor de itens) */
void atualizaItens(Item itens[], int n, float dt);      /* MAP */
int adicionaItem(Item itens[], int n, Item novo);

/* Desenho: mostra o item e nao altera o estado do jogo */
void desenhaItem(Item it);

#endif
