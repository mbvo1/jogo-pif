#include "raylib.h"
#include "cesta.h"

#define LARGURA 800
#define ALTURA 450

#define LARGURA_CESTA 100
#define ALTURA_CESTA 25
#define VELOCIDADE_CESTA 400.0f
#define DISTANCIA_DO_CHAO 10

#define TAMANHO_FONTE 20
#define MARGEM_HUD 10

/* Esboco do HUD: valores fixos por enquanto, a logica entra na Etapa 3.
   Pontos fica a esquerda, Tempo no centro e Vidas a direita. */
static void desenhaHud(void) {
    const char *pontos = "Pontos 0/30";
    const char *tempo = "Tempo 60";
    const char *vidas = "Vidas 3";
    int xTempo = (LARGURA - MeasureText(tempo, TAMANHO_FONTE)) / 2;
    int xVidas = LARGURA - MeasureText(vidas, TAMANHO_FONTE) - MARGEM_HUD;

    DrawText(pontos, MARGEM_HUD, MARGEM_HUD, TAMANHO_FONTE, DARKGRAY);
    DrawText(tempo, xTempo, MARGEM_HUD, TAMANHO_FONTE, DARKGRAY);
    DrawText(vidas, xVidas, MARGEM_HUD, TAMANHO_FONTE, DARKGRAY);
}

int main(void) {
    Cesta cesta = {
        { (LARGURA - LARGURA_CESTA) / 2.0f, ALTURA - ALTURA_CESTA - DISTANCIA_DO_CHAO,
          LARGURA_CESTA, ALTURA_CESTA },
        VELOCIDADE_CESTA
    };

    InitWindow(LARGURA, ALTURA, "Chuva de Frutas");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        /* Atualizacao: muda o estado, nao desenha */
        float dt = GetFrameTime();
        atualizaCesta(&cesta, dt, LARGURA);

        /* Desenho: mostra o estado, nao muda nada */
        BeginDrawing();
        ClearBackground(RAYWHITE);
        desenhaCesta(cesta);
        desenhaHud();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
