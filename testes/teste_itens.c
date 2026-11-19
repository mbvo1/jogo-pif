/* Testes das funcoes dos itens (puras e de colecao).
   Rodam sem abrir janela: so fazem calculo em cima de structs e vetores.
   Uso: make teste */

#include <stdio.h>
#include "../item.h"

static int falhas = 0;

/* Compara dois floats com uma pequena tolerancia */
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

/* Monta um item ja numa posicao, para nao depender do topo da tela */
static Item itemEm(float x, float y, TipoItem tipo) {
    Item it = criaItem(x, tipo);
    it.pos.y = y;
    return it;
}

/* Cesta usada nos testes: x 100..200, y 400..425 */
static const Rectangle CESTA = { 100, 400, 100, 25 };

static void testaCriaItem(void) {
    confereInt("criaItem maca vale 1", criaItem(50, MACA).valor, VALOR_MACA);
    confereInt("criaItem laranja vale 2", criaItem(50, LARANJA).valor, VALOR_LARANJA);
    confereInt("criaItem bomba vale 0", criaItem(50, BOMBA).valor, VALOR_BOMBA);
    confereFloat("criaItem guarda o x", criaItem(50, MACA).pos.x, 50);
    confereFloat("criaItem nasce acima da tela", criaItem(50, MACA).pos.y, -RAIO_ITEM);
}

static void testaCaiItem(void) {
    Item it = itemEm(50, 100, MACA);   /* velocidade 200 px/s */
    confereFloat("caiItem desce velocidade * dt", caiItem(it, 0.5f).pos.y, 200);
    confereFloat("caiItem com dt zero nao mexe", caiItem(it, 0).pos.y, 100);
    confereFloat("caiItem nao muda o x", caiItem(it, 0.5f).pos.x, 50);
    caiItem(it, 1.0f);
    confereFloat("caiItem nao altera o original", it.pos.y, 100);
}

static void testaSaiuDaTela(void) {
    confereInt("saiuDaTela no meio da tela", saiuDaTela(itemEm(50, 200, MACA), 450), 0);
    confereInt("saiuDaTela ainda aparecendo um pouco",
               saiuDaTela(itemEm(50, 450 + RAIO_ITEM, MACA), 450), 0);
    confereInt("saiuDaTela passou inteiro do chao",
               saiuDaTela(itemEm(50, 451 + RAIO_ITEM, MACA), 450), 1);
}

static void testaPegouItem(void) {
    float xLateral = CESTA.x - RAIO_ITEM;
    float yMeio = CESTA.y + CESTA.height / 2;
    /* raio 14: com o centro em y = 386 o circulo encosta no topo (400) */
    confereInt("pegouItem dentro da cesta", pegouItem(itemEm(150, 410, MACA), CESTA), 1);
    confereInt("pegouItem longe da cesta", pegouItem(itemEm(150, 100, MACA), CESTA), 0);
    confereInt("pegouItem ao lado da cesta", pegouItem(itemEm(300, 410, MACA), CESTA), 0);
    confereInt("pegouItem encostando na borda de cima",
               pegouItem(itemEm(150, 386, MACA), CESTA), 1);
    confereInt("pegouItem 1 pixel acima da borda",
               pegouItem(itemEm(150, 385, MACA), CESTA), 0);
    confereInt("pegouItem encostando na lateral esquerda",
               pegouItem(itemEm(xLateral, yMeio, MACA), CESTA), 1);
    confereInt("pegouItem a 1 pixel da lateral esquerda",
               pegouItem(itemEm(xLateral - 1, yMeio, MACA), CESTA), 0);
}

static void testaAtualizaItens(void) {
    Item itens[2] = { itemEm(50, 0, MACA), itemEm(80, 100, BOMBA) };
    atualizaItens(itens, 2, 0.5f);
    confereFloat("atualizaItens desce o primeiro", itens[0].pos.y, 100);
    confereFloat("atualizaItens desce o segundo", itens[1].pos.y, 200);
    atualizaItens(itens, 0, 0.5f);
    confereFloat("atualizaItens com n = 0 nao mexe", itens[0].pos.y, 100);
}

static void testaSomaPontosPegos(void) {
    Item itens[4] = {
        itemEm(150, 410, MACA),     /* pega: +1 */
        itemEm(120, 410, LARANJA),  /* pega: +2 */
        itemEm(180, 410, BOMBA),    /* pega, mas vale 0 */
        itemEm(500, 410, LARANJA)   /* nao pega */
    };
    confereInt("somaPontosPegos lista vazia da 0", somaPontosPegos(itens, 0, CESTA), 0);
    confereInt("somaPontosPegos maca + laranja da 3", somaPontosPegos(itens, 2, CESTA), 3);
    confereInt("somaPontosPegos bomba nao soma", somaPontosPegos(itens, 3, CESTA), 3);
    confereInt("somaPontosPegos ignora o que nao foi pego",
               somaPontosPegos(itens, 4, CESTA), 3);
}

static void testaContaBombasPegas(void) {
    Item itens[4] = {
        itemEm(150, 410, BOMBA),    /* pega */
        itemEm(150, 410, MACA),     /* fruta: nao conta */
        itemEm(500, 410, BOMBA),    /* nao pega */
        itemEm(110, 410, BOMBA)     /* pega */
    };
    confereInt("contaBombasPegas lista vazia da 0", contaBombasPegas(itens, 0, CESTA), 0);
    confereInt("contaBombasPegas conta so bombas pegas",
               contaBombasPegas(itens, 4, CESTA), 2);
}

static void testaRemoveItensMortos(void) {
    Item vazio[1];
    confereInt("removeItensMortos lista vazia da 0",
               removeItensMortos(vazio, 0, CESTA, 450), 0);

    Item nadaRemove[2] = { itemEm(10, 50, MACA), itemEm(20, 60, BOMBA) };
    confereInt("removeItensMortos sem nada a remover mantem 2",
               removeItensMortos(nadaRemove, 2, CESTA, 450), 2);

    /* x marca a ordem original: 1, 2, 3, 4, 5 */
    Item itens[5] = {
        itemEm(1, 50, MACA),        /* fica */
        itemEm(150, 410, LARANJA),  /* pego: sai */
        itemEm(3, 60, BOMBA),       /* fica */
        itemEm(4, 500, MACA),       /* saiu da tela: sai */
        itemEm(5, 70, LARANJA)      /* fica */
    };
    int n = removeItensMortos(itens, 5, CESTA, 450);
    confereInt("removeItensMortos remove do meio e sobra 3", n, 3);
    confereFloat("removeItensMortos mantem a ordem (1o)", itens[0].pos.x, 1);
    confereFloat("removeItensMortos mantem a ordem (2o)", itens[1].pos.x, 3);
    confereFloat("removeItensMortos mantem a ordem (3o)", itens[2].pos.x, 5);
}

static void testaAdicionaItem(void) {
    Item itens[MAX_ITENS];
    int n = 0;
    n = adicionaItem(itens, n, criaItem(10, MACA));
    confereInt("adicionaItem em vetor vazio da 1", n, 1);
    confereFloat("adicionaItem coloca no fim", itens[0].pos.x, 10);

    for (int i = 0; i < MAX_ITENS + 5; i++) {
        n = adicionaItem(itens, n, criaItem(20, LARANJA));
    }
    confereInt("adicionaItem para em MAX_ITENS", n, MAX_ITENS);
    confereInt("adicionaItem com vetor cheio devolve n",
               adicionaItem(itens, MAX_ITENS, criaItem(30, BOMBA)), MAX_ITENS);
}

int main(void) {
    testaCriaItem();
    testaCaiItem();
    testaSaiuDaTela();
    testaPegouItem();
    testaAtualizaItens();
    testaSomaPontosPegos();
    testaContaBombasPegas();
    testaRemoveItensMortos();
    testaAdicionaItem();

    if (falhas == 0) {
        printf("Todos os testes dos itens passaram.\n");
        return 0;
    }
    printf("%d teste(s) dos itens falharam.\n", falhas);
    return 1;
}
