#include "cesta.h"

/* PURA: o resultado depende so dos parametros e nada fora dela muda.
   direcao vale -1 (esquerda), 0 (parada) ou 1 (direita). */
float moveX(float x, float velocidade, int direcao, float dt) {
    return x + velocidade * direcao * dt;
}

/* PURA: mesma entrada sempre da a mesma saida.
   Devolve o x corrigido para a cesta nao sair da tela. */
float limitaX(float x, float largura, float larguraTela) {
    if (x < 0) {
        return 0;
    }
    if (x + largura > larguraTela) {
        return larguraTela - largura;
    }
    return x;
}

/* IMPURA: le o teclado, que e um estado de fora da funcao.
   Com as mesmas (zero) entradas, pode devolver valores diferentes. */
static int leDirecaoTeclado(void) {
    int direcao = 0;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        direcao = direcao + 1;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        direcao = direcao - 1;
    }
    return direcao;
}

/* IMPURA: le o teclado e altera a cesta recebida por ponteiro.
   O calculo em si fica todo nas funcoes puras. */
void atualizaCesta(Cesta *c, float dt, int larguraTela) {
    int direcao = leDirecaoTeclado();
    float novoX = moveX(c->corpo.x, c->velocidade, direcao, dt);
    c->corpo.x = limitaX(novoX, c->corpo.width, (float)larguraTela);
}

/* IMPURA (desenha na tela), mas nao altera o estado do jogo:
   recebe a cesta por valor, entao nem consegue mudar a original. */
void desenhaCesta(Cesta c) {
    DrawRectangleRec(c.corpo, BROWN);
    DrawRectangleLinesEx(c.corpo, 2, DARKBROWN);
}
