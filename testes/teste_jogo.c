/* Testes das regras puras do jogo (tempo e fim de partida).
   Rodam sem abrir janela. Uso: make teste */

#include <stdio.h>
#include "../jogo.h"

static int falhas = 0;

static void confereFloat(const char *nome, float obtido, float esperado) {
    float diferenca = obtido - esperado;
    if (diferenca > -0.01f && diferenca < 0.01f) {
        printf("ok     %s\n", nome);
    } else {
        printf("FALHOU %s: obtido %.2f, esperado %.2f\n", nome, obtido, esperado);
        falhas++;
    }
}

static void confereInt(const char *nome, int obtido, int esperado) {
    if (obtido == esperado) {
        printf("ok     %s\n", nome);
    } else {
        printf("FALHOU %s: obtido %d, esperado %d\n", nome, obtido, esperado);
        falhas++;
    }
}

static void testaVerificaResultado(void) {
    confereInt("verificaResultado vitoria com 30 pontos",
               verificaResultado(30, 3, 20), TELA_VITORIA);
    confereInt("verificaResultado vitoria passando de 30",
               verificaResultado(31, 1, 5), TELA_VITORIA);
    confereInt("verificaResultado derrota por vidas",
               verificaResultado(10, 0, 20), TELA_DERROTA);
    confereInt("verificaResultado derrota por tempo",
               verificaResultado(29, 2, 0), TELA_DERROTA);
    confereInt("verificaResultado continua jogando",
               verificaResultado(29, 1, 0.5f), TELA_JOGANDO);
    confereInt("verificaResultado comeco da partida",
               verificaResultado(0, VIDAS_INICIAIS, TEMPO_DE_JOGO), TELA_JOGANDO);
}

static void testaDescontaTempo(void) {
    confereFloat("descontaTempo tira dt", descontaTempo(60, 0.5f), 59.5f);
    confereFloat("descontaTempo com dt zero nao muda", descontaTempo(10, 0), 10);
    confereFloat("descontaTempo nao fica negativo", descontaTempo(0.01f, 0.5f), 0);
}

static void testaIniciaJogo(void) {
    Jogo j;
    j.pontos = 99;
    j.vidas = 0;
    j.numItens = 7;
    j.tela = TELA_DERROTA;
    iniciaJogo(&j);
    confereInt("iniciaJogo zera os pontos", j.pontos, 0);
    confereInt("iniciaJogo volta as vidas", j.vidas, VIDAS_INICIAIS);
    confereInt("iniciaJogo esvazia o vetor", j.numItens, 0);
    confereFloat("iniciaJogo volta o tempo", j.tempoRestante, TEMPO_DE_JOGO);
    confereInt("iniciaJogo vai para a tela de inicio", j.tela, TELA_INICIO);
}

int main(void) {
    testaVerificaResultado();
    testaDescontaTempo();
    testaIniciaJogo();

    if (falhas == 0) {
        printf("Todos os testes do jogo passaram.\n");
        return 0;
    }
    printf("%d teste(s) do jogo falharam.\n", falhas);
    return 1;
}
