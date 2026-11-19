/* Testes das regras e da atualizacao da partida.
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

/* Monta uma partida com uma maca alinhada ao centro da cesta. */
static Jogo partidaComMaca(float distancia) {
    Jogo j = {0};
    iniciaJogo(&j);
    j.tela = TELA_JOGANDO;
    Item maca = criaItem(j.cesta.corpo.x + j.cesta.corpo.width / 2, MACA);
    maca.pos.y = j.cesta.corpo.y - maca.raio - distancia;
    j.itens[0] = maca;
    j.numItens = 1;
    return j;
}

static void testaUltimoQuadro(void) {
    Jogo j = partidaComMaca(1.0f); /* contato depois de 5 ms */
    j.pontos = PONTOS_PARA_VENCER - VALOR_MACA;
    j.tempoRestante = 0.001f;
    atualizaJogo(&j, 0.016f);
    confereInt("contato depois do prazo perde", j.tela, TELA_DERROTA);
    confereInt("fruta fora do prazo nao pontua", j.pontos, PONTOS_PARA_VENCER - VALOR_MACA);

    j = partidaComMaca(1.0f);
    j.pontos = PONTOS_PARA_VENCER - VALOR_MACA;
    j.tempoRestante = 0.006f;
    atualizaJogo(&j, 0.016f);
    confereInt("contato antes do prazo vence", j.tela, TELA_VITORIA);

    j = partidaComMaca(VELOCIDADE_QUEDA / 128);
    j.pontos = PONTOS_PARA_VENCER - VALOR_MACA;
    j.tempoRestante = 1.0f / 128;
    atualizaJogo(&j, 0.016f);
    confereInt("primeiro toque no instante do fim perde", j.tela, TELA_DERROTA);
}

static void testaQuadroDemorado(void) {
    Jogo j = partidaComMaca(1.0f);
    atualizaJogo(&j, 0.4f);
    confereInt("maca cruzando a cesta pontua", j.pontos, VALOR_MACA);
    confereInt("maca capturada sai do vetor", j.numItens, 0);
}

static void testaGeracao(void) {
    Jogo j = {0};
    iniciaJogo(&j);
    j.tela = TELA_JOGANDO;
    atualizaJogo(&j, 2 * INTERVALO_ITENS);
    confereInt("dois intervalos geram dois itens", j.numItens, 2);
    if (j.numItens == 2) {
        confereFloat("primeiro item cai por um intervalo", j.itens[0].pos.y,
                     -RAIO_ITEM + VELOCIDADE_QUEDA * INTERVALO_ITENS);
        confereFloat("segundo item acaba de nascer", j.itens[1].pos.y, -RAIO_ITEM);
    }
    iniciaJogo(&j);
    j.tela = TELA_JOGANDO;
    atualizaJogo(&j, INTERVALO_ITENS * 1.25f);
    confereInt("nasce um item no meio do quadro", j.numItens, 1);
    if (j.numItens == 1) {
        confereFloat("item cai apenas a partir do nascimento", j.itens[0].pos.y,
                     -RAIO_ITEM + VELOCIDADE_QUEDA * INTERVALO_ITENS / 4);
    }
    confereFloat("preserva a fracao ate o proximo item", j.tempoProximoItem,
                 INTERVALO_ITENS * 0.75f);
}

int main(void) {
    testaUltimoQuadro();
    testaQuadroDemorado();
    testaGeracao();
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
