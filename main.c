#include "raylib.h"
#include "jogo.h"

/* O main so cuida da janela e do game loop.
   As regras ficam em jogo.c, item.c e cesta.c. */
int main(void) {
    Jogo jogo;

    InitWindow(LARGURA, ALTURA, "Chuva de Frutas");
    SetTargetFPS(60);
    iniciaJogo(&jogo);

    while (!WindowShouldClose()) {
        /* Atualizacao: muda o estado, nao desenha */
        float dt = GetFrameTime();
        atualizaJogo(&jogo, dt);

        /* Desenho: mostra o estado, nao muda nada */
        BeginDrawing();
        desenhaJogo(jogo);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
