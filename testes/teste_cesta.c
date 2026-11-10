/* Testes das funcoes puras da cesta.
   Rodam sem abrir janela, porque moveX e limitaX so fazem calculo.
   Uso: make teste */

#include <stdio.h>
#include "../cesta.h"

static int falhas = 0;

/* Compara dois floats com uma pequena tolerancia */
static void confere(const char *nome, float obtido, float esperado) {
    float diferenca = obtido - esperado;
    if (diferenca > -0.01f && diferenca < 0.01f) {
        printf("ok     %s\n", nome);
    } else {
        printf("FALHOU %s: obtido %.2f, esperado %.2f\n", nome, obtido, esperado);
        falhas++;
    }
}

static void testaMoveX(void) {
    confere("moveX anda para a direita", moveX(100, 400, 1, 0.5f), 300);
    confere("moveX anda para a esquerda", moveX(100, 400, -1, 0.25f), 0);
    confere("moveX parada nao muda x", moveX(100, 400, 0, 0.5f), 100);
    confere("moveX com dt zero nao muda x", moveX(100, 400, 1, 0), 100);
}

/* 1 segundo de jogo deve dar a mesma distancia em qualquer FPS */
static void testaMesmaDistanciaEmQualquerFps(void) {
    float x60 = 0;
    float x240 = 0;
    for (int i = 0; i < 60; i++) {
        x60 = moveX(x60, 400, 1, 1.0f / 60);
    }
    for (int i = 0; i < 240; i++) {
        x240 = moveX(x240, 400, 1, 1.0f / 240);
    }
    confere("1 segundo a 60 FPS anda 400 px", x60, 400);
    confere("1 segundo a 240 FPS anda 400 px", x240, 400);
}

static void testaLimitaX(void) {
    confere("limitaX segura na borda esquerda", limitaX(-5, 100, 800), 0);
    confere("limitaX segura na borda direita", limitaX(750, 100, 800), 700);
    confere("limitaX nao mexe no meio da tela", limitaX(350, 100, 800), 350);
    confere("limitaX encostada na esquerda", limitaX(0, 100, 800), 0);
    confere("limitaX encostada na direita", limitaX(700, 100, 800), 700);
}

int main(void) {
    testaMoveX();
    testaMesmaDistanciaEmQualquerFps();
    testaLimitaX();

    if (falhas == 0) {
        printf("Todos os testes passaram.\n");
        return 0;
    }
    printf("%d teste(s) falharam.\n", falhas);
    return 1;
}
