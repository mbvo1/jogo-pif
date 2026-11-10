#include "raylib.h"

#define LARGURA 800
#define ALTURA 450

/* Esboco do HUD: valores fixos por enquanto, a logica entra na Etapa 3 */
static void desenhaHud(void) {
    DrawText("Pontos 0/30", 10, 10, 20, DARKGRAY);
    DrawText("Tempo 60", 345, 10, 20, DARKGRAY);
    DrawText("Vidas 3", 700, 10, 20, DARKGRAY);
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "Chuva de Frutas");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        /* Desenho: mostra o estado, nao muda nada */
        BeginDrawing();
        ClearBackground(RAYWHITE);
        desenhaHud();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
