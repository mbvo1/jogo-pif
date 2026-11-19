#include "item.h"

/* PURA: so monta um Item com os valores iniciais.
   O item nasce logo acima da tela (y = -raio) para entrar descendo.
   Quem sorteia x e tipo e a camada impura (jogo.c), nao esta funcao. */
Item criaItem(float x, TipoItem tipo) {
    Item it;
    it.pos.x = x;
    it.pos.y = -RAIO_ITEM;
    it.raio = RAIO_ITEM;
    it.velocidade = VELOCIDADE_QUEDA;
    it.tipo = tipo;
    if (tipo == MACA) {
        it.valor = VALOR_MACA;
    } else if (tipo == LARANJA) {
        it.valor = VALOR_LARANJA;
    } else {
        it.valor = VALOR_BOMBA;
    }
    return it;
}

/* PURA: recebe o item por valor e devolve uma COPIA com a nova posicao.
   O original nao muda. Mesma entrada, mesma saida. */
Item caiItem(Item it, float dt) {
    it.pos.y = it.pos.y + it.velocidade * dt;
    return it;
}

/* PURA: so pergunta se o circulo do item encosta no retangulo da cesta.
   Usa a colisao pronta da Raylib, que e so calculo (nao precisa de janela).
   Encostar na borda ja conta como pegar. */
bool pegouItem(Item it, Rectangle cesta) {
    return CheckCollisionCircleRec(it.pos, it.raio, cesta);
}

/* MAP: aplica caiItem em cada item do vetor.
   Nao e pura no sentido estrito porque escreve no vetor recebido,
   mas nao le teclado, nao desenha e nao usa variavel global:
   o resultado depende so dos parametros, entao da para testar. */
void atualizaItens(Item itens[], int n, float dt) {
    for (int i = 0; i < n; i++) {
        itens[i] = caiItem(itens[i], dt);
    }
}

/* Coloca um item novo no fim do vetor e devolve o novo tamanho.
   Borda: se o vetor estiver cheio, nao adiciona e devolve n. */
int adicionaItem(Item itens[], int n, Item novo) {
    if (n >= MAX_ITENS) {
        return n;
    }
    itens[n] = novo;
    return n + 1;
}

/* FOLD: percorre o vetor juntando tudo em um numero so (a soma).
   Comeca do 0 e, para cada item pego, acumula o valor dele.
   Bomba vale 0, entao nao soma nada. Lista vazia devolve 0.
   PURA: o vetor e const, so lemos. */
int somaPontosPegos(const Item itens[], int n, Rectangle cesta) {
    int soma = 0;
    for (int i = 0; i < n; i++) {
        if (pegouItem(itens[i], cesta)) {
            soma = soma + itens[i].valor;
        }
    }
    return soma;
}

/* FILTER + FOLD: filtra so as bombas que foram pegas
   e dobra o resultado em uma contagem (soma 1 para cada uma).
   PURA: o vetor e const, so lemos. */
int contaBombasPegas(const Item itens[], int n, Rectangle cesta) {
    int bombas = 0;
    for (int i = 0; i < n; i++) {
        if (itens[i].tipo == BOMBA && pegouItem(itens[i], cesta)) {
            bombas = bombas + 1;
        }
    }
    return bombas;
}

/* IMPURA (desenha na tela), mas recebe o item por valor:
   nao consegue mudar o item original. Formas simples, sem sprites. */
void desenhaItem(Item it) {
    if (it.tipo == MACA) {
        DrawCircleV(it.pos, it.raio, RED);
        DrawRectangle((int)it.pos.x - 1, (int)(it.pos.y - it.raio) - 5, 3, 7, BROWN);
        DrawCircle((int)it.pos.x + 5, (int)(it.pos.y - it.raio) - 2, 4, GREEN);
    } else if (it.tipo == LARANJA) {
        DrawCircleV(it.pos, it.raio, ORANGE);
        DrawCircle((int)it.pos.x, (int)(it.pos.y - it.raio) + 2, 3, DARKGREEN);
    } else {
        Vector2 pontaPavio = { it.pos.x + it.raio * 0.7f, it.pos.y - it.raio * 1.4f };
        DrawCircleV(it.pos, it.raio, BLACK);
        DrawLineEx((Vector2){ it.pos.x, it.pos.y - it.raio }, pontaPavio, 3, BROWN);
        DrawCircleV(pontaPavio, 4, YELLOW);
    }
}
