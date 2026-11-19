#include "jogo.h"

#define LARGURA_CESTA 100
#define ALTURA_CESTA 25
#define VELOCIDADE_CESTA 400.0f
#define DISTANCIA_DO_CHAO 10

#define TAMANHO_FONTE 20
#define MARGEM_HUD 10

/* Muda o jogo recebido por ponteiro, mas nao le teclado nem sorteia:
   sempre deixa o jogo no mesmo estado inicial. */
void iniciaJogo(Jogo *j) {
    j->cesta.corpo.x = (LARGURA - LARGURA_CESTA) / 2.0f;
    j->cesta.corpo.y = ALTURA - ALTURA_CESTA - DISTANCIA_DO_CHAO;
    j->cesta.corpo.width = LARGURA_CESTA;
    j->cesta.corpo.height = ALTURA_CESTA;
    j->cesta.velocidade = VELOCIDADE_CESTA;
    j->numItens = 0;
    j->pontos = 0;
    j->vidas = VIDAS_INICIAIS;
    j->tempoProximoItem = INTERVALO_ITENS;
}

/* IMPURA: usa GetRandomValue, entao cada chamada da um item diferente.
   O sorteio fica aqui, na camada impura; criaItem continua pura. */
static Item sorteiaItem(void) {
    float x = (float)GetRandomValue((int)RAIO_ITEM, LARGURA - (int)RAIO_ITEM);
    int sorteio = GetRandomValue(1, 100);
    TipoItem tipo = LARANJA;
    if (sorteio <= 25) {
        tipo = BOMBA;      /* ~25% de bomba */
    } else if (sorteio <= 70) {
        tipo = MACA;       /* ~45% de maca, o resto e laranja */
    }
    return criaItem(x, tipo);
}

/* IMPURA: muda o jogo. Quando o timer zera, nasce um item novo. */
static void geraItemSeForHora(Jogo *j, float dt) {
    j->tempoProximoItem = j->tempoProximoItem - dt;
    if (j->tempoProximoItem <= 0) {
        j->numItens = adicionaItem(j->itens, j->numItens, sorteiaItem());
        j->tempoProximoItem = INTERVALO_ITENS;
    }
}

/* IMPURA: le o teclado (pela cesta) e muda o estado do jogo.
   Cada passo chama uma funcao pequena; as regras ficam nas funcoes puras. */
void atualizaJogo(Jogo *j, float dt) {
    atualizaCesta(&j->cesta, dt, LARGURA);
    geraItemSeForHora(j, dt);
    atualizaItens(j->itens, j->numItens, dt);                          /* map */
    j->pontos += somaPontosPegos(j->itens, j->numItens, j->cesta.corpo);   /* fold */
    j->vidas -= contaBombasPegas(j->itens, j->numItens, j->cesta.corpo);   /* filter + fold */
}

/* HUD: Pontos a esquerda e Vidas a direita */
static void desenhaHud(Jogo j) {
    const char *pontos = TextFormat("Pontos %d/%d", j.pontos, PONTOS_PARA_VENCER);
    DrawText(pontos, MARGEM_HUD, MARGEM_HUD, TAMANHO_FONTE, DARKGRAY);

    const char *vidas = TextFormat("Vidas %d", j.vidas);
    int xVidas = LARGURA - MeasureText(vidas, TAMANHO_FONTE) - MARGEM_HUD;
    DrawText(vidas, xVidas, MARGEM_HUD, TAMANHO_FONTE, DARKGRAY);
}

/* IMPURA (desenha), mas recebe o jogo por valor: nao altera o estado */
void desenhaJogo(Jogo j) {
    ClearBackground(RAYWHITE);
    for (int i = 0; i < j.numItens; i++) {
        desenhaItem(j.itens[i]);
    }
    desenhaCesta(j.cesta);
    desenhaHud(j);
}
