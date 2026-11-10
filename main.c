#include "raylib.h"
#include "cesta.h"

#define LARGURA 800
#define ALTURA 450

#define LARGURA_CESTA 100
#define ALTURA_CESTA 25
#define VELOCIDADE_CESTA 400.0f

/* Esboco do HUD: valores fixos por enquanto, a logica entra na Etapa 3 */
static void desenhaHud(void) {
    DrawText("Pontos 0/30", 10, 10, 20, DARKGRAY);
    DrawText("Tempo 60", 345, 10, 20, DARKGRAY);
    DrawText("Vidas 3", 700, 10, 20, DARKGRAY);
}

int main(void) {
    Cesta cesta = {
        { (LARGURA - LARGURA_CESTA) / 2.0f, ALTURA - ALTURA_CESTA - 10,
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
