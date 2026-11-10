# Chuva de Frutas

Jogo 2D em C com a [Raylib](https://www.raylib.com/), feito para o Projeto Integrador de **Programação Imperativa e Funcional (PIF)**, CESAR School, 2026.2.

**Dupla:** Marcelo Bresani Victor de Oliveira e João Cláudio Beltrão

---

## Etapa 1 — Proposta

### O jogo

O jogador controla uma cesta na parte de baixo da tela. Frutas e bombas caem do topo. A cesta precisa pegar as frutas e desviar das bombas. Para vencer, é preciso fazer **30 pontos antes de o tempo (60 s) acabar**.

### Objetos

| Objeto | Representação (struct) | Quantidade |
|---|---|---|
| Cesta (jogador) | posição, tamanho, velocidade | 1 |
| Fruta | posição, velocidade de queda, valor em pontos, ativo ou não | vetor de até N itens |
| Bomba | posição, velocidade de queda, ativo ou não | no mesmo vetor das frutas, com um campo `tipo` |
| Estado do jogo | pontos, vidas, tempo restante, tela atual | 1 |

### Regras

- A cesta anda para a esquerda e a direita com as setas (ou A/D) e não sai da tela.
- A cada intervalo de tempo, nasce um item novo no topo, em posição horizontal aleatória.
- **Pegar uma fruta:** soma os pontos da fruta. Frutas diferentes podem valer pontos diferentes.
- **Pegar uma bomba:** perde 1 vida.
- **Deixar a fruta cair no chão:** a fruta some, sem penalidade.
- O jogador começa com **3 vidas**.
- **Vitória:** chegar a 30 pontos antes de o tempo acabar.
- **Derrota:** as vidas chegam a 0 ou o tempo acaba antes dos 30 pontos.
- **Telas:** início → jogando → fim (vitória ou derrota). Na tela final, ENTER reinicia.

> Os valores (30 pontos, 60 s, 3 vidas) podem ser ajustados depois de testar a jogabilidade.

### Onde entram os padrões funcionais

| Padrão | Uso no jogo |
|---|---|
| **map** | A cada quadro, aplicar a queda a todos os itens: `pos.y += vel * dt` |
| **filter** | Remover do vetor os itens coletados ou que saíram da tela (compactar os ativos) |
| **fold** | Somar os pontos das frutas coletadas no quadro e contar as bombas atingidas |

As regras ficam em **funções puras** (por exemplo, "esta fruta colidiu com a cesta?" e "nova posição dada a velocidade e o `dt`"). O que tem efeito colateral (desenhar e ler o teclado) fica separado.

### Esboço da tela

```
+--------------------------------------------------+
| Pontos: 12/30      Tempo: 41s       Vidas: ♥ ♥ ♥ |
|                                                  |
|        (o)                    (o)                |
|                  [X]                             |
|   (o)                               [X]          |
|                        (o)                       |
|                                                  |
|                                                  |
|                  \__________/                    |
+--------------------------------------------------+
   (o) = fruta    [X] = bomba    \___/ = cesta
```

---

## Etapas

- [x] **E1** — Proposta (este README)
- [ ] **E2** — Janela + game loop a 60 FPS com `GetFrameTime()` + cesta controlável
- [ ] **E3** — Vetor de itens + colisões + pontuação
- [ ] **E4** — Versão final + telas + apresentação

---

## Como compilar e rodar

Pré-requisito: Raylib instalada. No Windows, usamos o instalador oficial em `C:\raylib`.

```
make
./jogo
```

No Windows, o executável é `jogo.exe`.

## Controles

| Tecla | Ação |
|---|---|
| ← / → ou A / D | Mover a cesta |
| ENTER | Começar / reiniciar |
| ESC | Sair |
