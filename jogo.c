#include "jogo.h"

#define LARGURA_CESTA 100
#define ALTURA_CESTA 25
#define VELOCIDADE_CESTA 400.0f
#define DISTANCIA_DO_CHAO 10

#define CHANCE_BOMBA 25        /* em %, de 1 a 100 */
#define CHANCE_MACA 45         /* em %, o que sobra e laranja */

#define TAMANHO_FONTE 20
#define FONTE_TITULO 40
#define MARGEM_HUD 10

/* ---------- Regras puras ---------- */

/* PURA: devolve o tempo que sobra, sem deixar ficar negativo */
float descontaTempo(float tempoRestante, float dt) {
    float novo = tempoRestante - dt;
    if (novo < 0) {
        return 0;
    }
    return novo;
}

/* PURA: decide a tela so com os tres numeros recebidos.
   Ordem das regras: chegar aos pontos vence (mesmo que no mesmo quadro
   tenha perdido a ultima vida); senao, sem vidas ou sem tempo perde. */
Tela verificaResultado(int pontos, int vidas, float tempoRestante) {
    if (pontos >= PONTOS_PARA_VENCER) {
        return TELA_VITORIA;
    }
    if (vidas <= 0 || tempoRestante <= 0) {
        return TELA_DERROTA;
    }
    return TELA_JOGANDO;
}

/* ---------- Atualizacao ---------- */

/* Muda o jogo recebido por ponteiro, mas nao le teclado nem sorteia:
   sempre deixa o jogo no mesmo estado inicial, na tela de inicio. */
void iniciaJogo(Jogo *j) {
    j->cesta.corpo.x = (LARGURA - LARGURA_CESTA) / 2.0f;
    j->cesta.corpo.y = ALTURA - ALTURA_CESTA - DISTANCIA_DO_CHAO;
    j->cesta.corpo.width = LARGURA_CESTA;
    j->cesta.corpo.height = ALTURA_CESTA;
    j->cesta.velocidade = VELOCIDADE_CESTA;
    j->numItens = 0;
    j->pontos = 0;
    j->vidas = VIDAS_INICIAIS;
    j->tempoRestante = TEMPO_DE_JOGO;
    j->tempoProximoItem = INTERVALO_ITENS;
    j->tela = TELA_INICIO;
}

/* IMPURA: usa GetRandomValue, entao cada chamada da um item diferente.
   O sorteio fica aqui, na camada impura; criaItem continua pura. */
static Item sorteiaItem(void) {
    float x = (float)GetRandomValue((int)RAIO_ITEM, LARGURA - (int)RAIO_ITEM);
    int sorteio = GetRandomValue(1, 100);
    TipoItem tipo = LARANJA;
    if (sorteio <= CHANCE_BOMBA) {
        tipo = BOMBA;
    } else if (sorteio <= CHANCE_BOMBA + CHANCE_MACA) {
        tipo = MACA;
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

/* IMPURA: um quadro da partida. Cada linha chama uma funcao pequena;
   as regras de verdade ficam nas funcoes puras. */
static void atualizaJogando(Jogo *j, float dt) {
    Rectangle cesta;

    atualizaCesta(&j->cesta, dt, LARGURA);
    j->tempoRestante = descontaTempo(j->tempoRestante, dt);
    geraItemSeForHora(j, dt);

    cesta = j->cesta.corpo;
    atualizaItens(j->itens, j->numItens, dt);                          /* map */
    j->pontos += somaPontosPegos(j->itens, j->numItens, cesta);        /* fold */
    j->vidas -= contaBombasPegas(j->itens, j->numItens, cesta);        /* filter+fold */
    j->numItens = removeItensMortos(j->itens, j->numItens, cesta, ALTURA); /* filter */

    j->tela = verificaResultado(j->pontos, j->vidas, j->tempoRestante);
}

/* IMPURA: le o teclado e muda o estado conforme a tela atual */
void atualizaJogo(Jogo *j, float dt) {
    switch (j->tela) {
    case TELA_INICIO:
    case TELA_VITORIA:
    case TELA_DERROTA:
        if (IsKeyPressed(KEY_ENTER)) {
            iniciaJogo(j);
            j->tela = TELA_JOGANDO;
        }
        break;
    case TELA_JOGANDO:
        atualizaJogando(j, dt);
        break;
    }
}

/* ---------- Desenho (nao altera o estado) ---------- */

/* Escreve um texto centralizado na horizontal */
static void textoCentralizado(const char *texto, int y, int tamanho, Color cor) {
    int x = (LARGURA - MeasureText(texto, tamanho)) / 2;
    DrawText(texto, x, y, tamanho, cor);
}

/* HUD: Pontos a esquerda, Tempo no centro e Vidas a direita */
static void desenhaHud(Jogo j) {
    const char *pontos = TextFormat("Pontos %d/%d", j.pontos, PONTOS_PARA_VENCER);
    const char *vidas = TextFormat("Vidas %d", j.vidas);
    int xVidas = LARGURA - MeasureText(vidas, TAMANHO_FONTE) - MARGEM_HUD;

    DrawText(pontos, MARGEM_HUD, MARGEM_HUD, TAMANHO_FONTE, DARKGRAY);
    textoCentralizado(TextFormat("Tempo %d", (int)j.tempoRestante),
                      MARGEM_HUD, TAMANHO_FONTE, DARKGRAY);
    DrawText(vidas, xVidas, MARGEM_HUD, TAMANHO_FONTE, DARKGRAY);
}

static void desenhaTelaInicio(void) {
    textoCentralizado("Chuva de Frutas", 90, FONTE_TITULO, DARKGREEN);
    textoCentralizado("Pegue as frutas e desvie das bombas!", 170, TAMANHO_FONTE, DARKGRAY);
    textoCentralizado(TextFormat("Faca %d pontos em %d segundos. Voce tem %d vidas.",
                                 PONTOS_PARA_VENCER, (int)TEMPO_DE_JOGO, VIDAS_INICIAIS),
                      200, TAMANHO_FONTE, DARKGRAY);
    textoCentralizado("Maca = 1 ponto   Laranja = 2 pontos   Bomba = -1 vida",
                      230, TAMANHO_FONTE, DARKGRAY);
    textoCentralizado("Setas ou A/D para mover", 280, TAMANHO_FONTE, GRAY);
    textoCentralizado("ENTER para jogar", 340, TAMANHO_FONTE, MAROON);
}

static void desenhaTelaFim(Jogo j) {
    if (j.tela == TELA_VITORIA) {
        textoCentralizado("Voce venceu!", 120, FONTE_TITULO, DARKGREEN);
        textoCentralizado(TextFormat("Sobraram %d segundos", (int)j.tempoRestante),
                          220, TAMANHO_FONTE, DARKGRAY);
    } else {
        textoCentralizado("Fim de jogo", 120, FONTE_TITULO, MAROON);
        if (j.vidas <= 0) {
            textoCentralizado("Suas vidas acabaram", 220, TAMANHO_FONTE, DARKGRAY);
        } else {
            textoCentralizado("O tempo acabou", 220, TAMANHO_FONTE, DARKGRAY);
        }
    }
    textoCentralizado(TextFormat("Pontos: %d/%d", j.pontos, PONTOS_PARA_VENCER),
                      190, TAMANHO_FONTE, DARKGRAY);
    textoCentralizado("ENTER para jogar de novo", 300, TAMANHO_FONTE, MAROON);
}

static void desenhaPartida(Jogo j) {
    for (int i = 0; i < j.numItens; i++) {
        desenhaItem(j.itens[i]);
    }
    desenhaCesta(j.cesta);
    desenhaHud(j);
}

/* IMPURA (desenha), mas recebe o jogo por valor: nao altera o estado */
void desenhaJogo(Jogo j) {
    ClearBackground(RAYWHITE);
    switch (j.tela) {
    case TELA_INICIO:
        desenhaTelaInicio();
        break;
    case TELA_JOGANDO:
        desenhaPartida(j);
        break;
    case TELA_VITORIA:
    case TELA_DERROTA:
        desenhaTelaFim(j);
        break;
    }
}
