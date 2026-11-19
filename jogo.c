#include <math.h>
#include "jogo.h"

#define LARGURA_CESTA 100
#define ALTURA_CESTA 25
#define VELOCIDADE_CESTA 400.0f
#define DISTANCIA_DO_CHAO 10

/* A 128 passos/s, um item anda no maximo 1,57 pixel por passo. */
#define PASSO_MAXIMO (1.0 / 128)

#define CHANCE_BOMBA 25        /* em %, de 1 a 100 */
#define CHANCE_MACA 45         /* em %, o que sobra e laranja */

#define TAMANHO_FONTE 20
#define FONTE_TITULO 40
#define MARGEM_HUD 10
#define RAIO_CORACAO 5
#define LARGURA_CORACAO (4 * RAIO_CORACAO)
#define ESPACAMENTO_CORACOES 6

/* ---------- Regras puras ---------- */

/* PURA: devolve o tempo que sobra, sem deixar ficar negativo */
double descontaTempo(double tempoRestante, double dt) {
    double novo = tempoRestante - dt;
    if (novo < 0) {
        return 0;
    }
    return novo;
}

/* PURA: decide a tela so com os tres numeros recebidos.
   Ordem das regras: chegar aos pontos vence (mesmo que no mesmo passo
   tenha perdido a ultima vida); senao, sem vidas ou sem tempo perde. */
Tela verificaResultado(int pontos, int vidas, double tempoRestante) {
    if (pontos >= PONTOS_PARA_VENCER) {
        return TELA_VITORIA;
    }
    if (vidas <= 0 || tempoRestante <= 0) {
        return TELA_DERROTA;
    }
    return TELA_JOGANDO;
}

/* ---------- Atualizacao ---------- */

/* IMPURA: muda o jogo recebido por ponteiro, mas nao le teclado nem sorteia:
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

/* PURA: projeta o movimento sem incluir o instante final. Se arredondar
   para float atingir ou passar da chegada, usa a coordenada anterior. */
static float posicaoAntesDoFim(float inicio, double velocidade, double dt) {
    double chegada = inicio + velocidade * dt;
    float posicao = (float)chegada;
    if ((velocidade > 0 && posicao >= chegada) ||
        (velocidade < 0 && posicao <= chegada)) {
        return nextafterf(posicao, inicio);
    }
    return posicao;
}

/* IMPURA: move a cesta e os itens e aplica as capturas deste passo. */
static void moveECaptura(Jogo *j, double dt, bool ultimoPasso) {
    float xInicial = j->cesta.corpo.x;
    atualizaCesta(&j->cesta, (float)dt, LARGURA);
    Rectangle cesta = j->cesta.corpo;
    if (ultimoPasso) {
        /* Conta apenas o contato anterior ao apito. Projeta a cesta antes
           de limitar na borda, pois ela pode ter chegado ali mais cedo. */
        if (cesta.x != xInicial) {
            double velocidade = cesta.x > xInicial ? j->cesta.velocidade : -j->cesta.velocidade;
            cesta.x = limitaX(posicaoAntesDoFim(xInicial, velocidade, dt), cesta.width, LARGURA);
        }
        for (int i = 0; i < j->numItens; i++) {
            Item *item = &j->itens[i];
            item->pos.y = posicaoAntesDoFim(item->pos.y, item->velocidade, dt);
        }
    } else {
        atualizaItens(j->itens, j->numItens, (float)dt);               /* map */
    }
    j->pontos += somaPontosPegos(j->itens, j->numItens, cesta);        /* fold */
    j->vidas -= contaBombasPegas(j->itens, j->numItens, cesta);        /* filter+fold */
    j->numItens = removeItensMortos(j->itens, j->numItens, cesta, ALTURA); /* filter */
}

/* IMPURA: avanca ate o fim do quadro, o nascimento de um item ou o fim
   da partida. Cada item so comeca a cair no passo seguinte ao nascimento. */
static void atualizaJogando(Jogo *j, float dt) {
    double falta = fmin(dt, j->tempoRestante);
    while (falta > 0 && j->tela == TELA_JOGANDO) {
        double passo = fmin(falta, fmin(PASSO_MAXIMO, j->tempoProximoItem));
        moveECaptura(j, passo, passo == j->tempoRestante);
        j->tempoRestante = descontaTempo(j->tempoRestante, passo);
        j->tempoProximoItem -= passo;
        falta -= passo;
        j->tela = verificaResultado(j->pontos, j->vidas, j->tempoRestante);
        if (j->tempoProximoItem <= 0 && j->tela == TELA_JOGANDO) {
            j->numItens = adicionaItem(j->itens, j->numItens, sorteiaItem());
            j->tempoProximoItem = INTERVALO_ITENS;
        }
    }
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

/* IMPURA: desenha um texto centralizado na horizontal */
static void textoCentralizado(const char *texto, int y, int tamanho, Color cor) {
    int x = (LARGURA - MeasureText(texto, tamanho)) / 2;
    DrawText(texto, x, y, tamanho, cor);
}

/* IMPURA: desenha na tela usando apenas a posicao e o estado do coracao. */
static void desenhaCoracao(int x, int y, bool cheio) {
    Color cor = cheio ? RED : LIGHTGRAY;
    int meioY = y + RAIO_CORACAO;
    Vector2 esquerda = { x, meioY };
    Vector2 ponta = { x + LARGURA_CORACAO / 2, y + 4 * RAIO_CORACAO };
    Vector2 direita = { x + LARGURA_CORACAO, meioY };

    DrawCircle(x + RAIO_CORACAO, meioY, RAIO_CORACAO, cor);
    DrawCircle(x + LARGURA_CORACAO - RAIO_CORACAO, meioY, RAIO_CORACAO, cor);
    DrawTriangle(esquerda, ponta, direita, cor);
}

/* IMPURA: desenha pontos, tempo e um coracao por vida inicial. */
static void desenhaHud(Jogo j) {
    const char *pontos = TextFormat("Pontos %d/%d", j.pontos, PONTOS_PARA_VENCER);
    int larguraVidas = VIDAS_INICIAIS * LARGURA_CORACAO
                     + (VIDAS_INICIAIS - 1) * ESPACAMENTO_CORACOES;
    int x = LARGURA - MARGEM_HUD - larguraVidas;

    DrawText(pontos, MARGEM_HUD, MARGEM_HUD, TAMANHO_FONTE, DARKGRAY);
    textoCentralizado(TextFormat("Tempo %d", (int)ceil(j.tempoRestante)),
                      MARGEM_HUD, TAMANHO_FONTE, DARKGRAY);
    for (int i = 0; i < VIDAS_INICIAIS; i++) {
        desenhaCoracao(x, MARGEM_HUD, i < j.vidas);
        x += LARGURA_CORACAO + ESPACAMENTO_CORACOES;
    }
}

/* IMPURA: desenha as instrucoes na janela. */
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

/* IMPURA: desenha o resultado da partida na janela. */
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

/* IMPURA: desenha os itens, a cesta e o HUD na janela. */
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
