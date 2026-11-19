# Chuva de Frutas

Jogo 2D em C com a [Raylib](https://www.raylib.com/), feito para o Projeto Integrador de **Programação Imperativa e Funcional (PIF)**, CESAR School, 2026.2.

**Dupla:** Marcelo Bresani Victor de Oliveira e João Cláudio Beltrão

---

## Etapas

- [x] **E1** — Proposta (seção abaixo)
- [x] **E2** — Janela + game loop a 60 FPS com `GetFrameTime()` + cesta controlável
- [x] **E3** — Vetor de itens + colisões com a Raylib + pontuação
- [x] **E4** — Versão final jogável + telas de início e fim + README completo

---

## O jogo

O jogador controla uma cesta na parte de baixo da tela. Maçãs, laranjas e bombas caem do topo. A cesta precisa pegar as frutas e desviar das bombas. Para vencer, é preciso fazer **30 pontos antes de o tempo (60 s) acabar**.

### Regras

- A cesta anda para a esquerda e a direita e não sai da tela.
- A cada 0,8 s nasce um item novo no topo, em posição horizontal aleatória. Chance de cada item: 45% maçã, 30% laranja, 25% bomba.
- **Pegar uma maçã:** +1 ponto. **Pegar uma laranja:** +2 pontos.
- **Pegar uma bomba:** perde 1 vida.
- **Deixar um item cair no chão:** ele some, sem penalidade.
- O jogador começa com **3 vidas**. No canto superior direito, cada coração vermelho representa uma vida restante e cada coração cinza, uma vida perdida.
- **Vitória:** chegar a 30 pontos antes de o tempo acabar.
- **Derrota:** as vidas chegam a 0, ou o tempo acaba antes dos 30 pontos.
- **Telas:** início → jogando → vitória ou derrota. ENTER começa o jogo na tela de início e reinicia nas telas finais.

Os números do jogo ficam em `#define` no começo dos arquivos (`jogo.h`, `jogo.c`, `item.h`), então dá para mudar a dificuldade sem procurar números soltos no código.

### Controles

| Tecla | Ação |
|---|---|
| ← / → ou A / D | Mover a cesta |
| ENTER | Começar (tela de início) / reiniciar (telas de vitória e derrota) |
| ESC | Sair |

---

## Como compilar e rodar

Pré-requisito: Raylib instalada. No Windows, usamos o instalador oficial em `C:\raylib`, que já traz o `gcc` e o `make` no w64devkit.

No Linux, WSL ou macOS:

```
make
./jogo
```

No Windows (PowerShell), o executável é `jogo.exe`:

```
$env:Path = "C:\raylib\w64devkit\bin;$env:Path"
make
.\jogo.exe
```

| Comando | O que faz |
|---|---|
| `make` | Compila o jogo (todos os `.c` da pasta) com `-Wall -Wextra -std=c99` |
| `make teste` | Compila e roda todos os testes, sem abrir janela |
| `make clean` | Apaga os executáveis |

---

## Estrutura do código

| Arquivo | Responsabilidade |
|---|---|
| `main.c` | Só abre a janela e roda o game loop: lê o `dt`, chama `atualizaJogo` e depois `desenhaJogo`. |
| `jogo.h` / `jogo.c` | Estado do jogo (`struct Jogo`), telas (`enum Tela`), regras de tempo e de fim de jogo, sorteio dos itens, HUD e telas de início/vitória/derrota. |
| `item.h` / `item.c` | O `struct Item` (maçã, laranja ou bomba) e todas as funções sobre um item e sobre o vetor de itens: queda, colisão, pontuação, remoção, desenho. |
| `cesta.h` / `cesta.c` | A cesta do jogador: movimento com `dt`, limite nas bordas e leitura do teclado. |
| `testes/teste_cesta.c` | Testes de `moveX` e `limitaX`. |
| `testes/teste_itens.c` | Testes das funções dos itens e do vetor. |
| `testes/teste_jogo.c` | Testes de `verificaResultado`, `descontaTempo` e `iniciaJogo`. |
| `Makefile` | Compila o jogo e os testes no Windows, Linux/WSL e macOS. |

Em cada quadro o jogo faz duas coisas separadas:

1. **Atualizar** (`atualizaJogo`): muda o estado, não desenha nada.
2. **Desenhar** (`desenhaJogo`): mostra o estado e não muda nada. Recebe o `Jogo` **por valor**, então nem consegue alterar o original.

Durante a partida, `atualizaJogo` faz, nesta ordem:

```
atualizaCesta                         -> move a cesta pelo teclado
descontaTempo                         -> tira dt do tempo restante
geraItemSeForHora                     -> sorteia e adiciona item quando o timer zera
atualizaItens        (map)            -> todos os itens descem
somaPontosPegos      (fold)           -> soma os pontos das frutas pegas
contaBombasPegas     (filter + fold)  -> tira 1 vida por bomba pega
removeItensMortos    (filter)         -> tira do vetor os itens pegos ou fora da tela
verificaResultado                     -> continua, vitória ou derrota
```

---

## Padrões funcionais no código

Os itens que estão caindo ficam em um vetor `Item itens[MAX_ITENS]` com um contador `numItens`. As operações sobre esse vetor seguem os padrões vistos em aula:

| Padrão | Função | Arquivo | O que faz |
|---|---|---|---|
| **map** | `atualizaItens` | `item.c` | Aplica `caiItem` a cada item: `pos.y += velocidade * dt`. Mesma operação em todos, um resultado para cada item. |
| **fold** | `somaPontosPegos` | `item.c` | Junta o vetor em **um número**: começa em 0 e acumula o `valor` de cada item pego. |
| **filter + fold** | `contaBombasPegas` | `item.c` | Filtra só as bombas pegas e dobra o resultado em uma contagem. |
| **filter** | `removeItensMortos` | `item.c` | Mantém só os itens que não foram pegos e ainda estão na tela, compactando o vetor (sem buracos e na mesma ordem). Devolve o novo tamanho. |

Bordas tratadas:

- **Vetor cheio:** `adicionaItem` não escreve fora do vetor; se `n == MAX_ITENS`, devolve `n` sem adicionar.
- **Lista vazia:** com `n = 0` os laços não rodam; `somaPontosPegos` e `contaBombasPegas` devolvem 0 e `removeItensMortos` devolve 0.
- **Item saindo da tela:** `saiuDaTela` detecta quando o item passou inteiro do chão e o `filter` remove.
- **Cesta na borda:** `limitaX` impede a cesta de sair da tela.
- **Tempo:** `descontaTempo` não deixa o tempo ficar negativo.

---

## Funções puras e testes

Uma função **pura** depende só dos seus parâmetros e não muda nada fora dela: com a mesma entrada, dá sempre a mesma saída. No nosso código, cada função tem um comentário dizendo se é PURA ou IMPURA e por quê.

| Função | Arquivo | Por que é pura |
|---|---|---|
| `moveX`, `limitaX` | `cesta.c` | Só fazem conta com os números recebidos. |
| `criaItem` | `item.c` | Só monta um `Item`. Quem sorteia a posição e o tipo é `sorteiaItem` em `jogo.c` (impura). |
| `caiItem` | `item.c` | Recebe o item por valor e devolve uma cópia com a nova posição. |
| `pegouItem` | `item.c` | Usa `CheckCollisionCircleRec` da Raylib, que é só cálculo de geometria. |
| `saiuDaTela` | `item.c` | Só compara números. |
| `somaPontosPegos`, `contaBombasPegas` | `item.c` | Recebem o vetor como `const` e só leem. |
| `verificaResultado`, `descontaTempo` | `jogo.c` | Decidem só com os números recebidos. |

`atualizaItens`, `removeItensMortos` e `adicionaItem` escrevem no vetor que recebem (por isso não são puras no sentido estrito), mas não leem teclado, não desenham, não sorteiam e não usam variável global: o resultado depende só dos parâmetros. Por isso também dá para testar.

As funções **impuras** ficam isoladas: ler o teclado (`leDirecaoTeclado`, `IsKeyPressed`), sortear (`sorteiaItem` com `GetRandomValue`) e desenhar (`desenha...`).

### Como rodar os testes

```
make teste
```

Isso compila `testes/teste_cesta.c`, `testes/teste_itens.c` e `testes/teste_jogo.c` junto com os módulos do jogo (todos os `.c` **menos** o `main.c`, porque cada teste tem o seu próprio `main`) e roda os três. Cada caso imprime `ok` ou `FALHOU`; se algum falhar, o `make` para com erro.

Os testes não abrem janela. Isso só é possível porque as regras estão em funções puras: para saber se a cesta pegou uma maçã, basta montar um `Item` e um `Rectangle` e chamar `pegouItem`, sem precisar de teclado, tela ou sorte. Alguns casos testados:

- `caiItem` com `dt = 0` não mexe o item, e não altera o original.
- `pegouItem` pega, não pega, e pega quando o círculo só encosta na borda.
- `somaPontosPegos`: lista vazia dá 0; maçã + laranja dá 3; bomba não soma.
- `removeItensMortos`: lista vazia; remove do meio mantendo a ordem; nada a remover.
- `adicionaItem` com o vetor cheio não passa de `MAX_ITENS`.
- `verificaResultado`: vitória, derrota por vidas, derrota por tempo, continua jogando.

---

## Etapa 1 — Proposta (original)

### Objetos

| Objeto | Representação (struct) | Quantidade |
|---|---|---|
| Cesta (jogador) | posição, tamanho, velocidade | 1 |
| Fruta | posição, velocidade de queda, valor em pontos, ativo ou não | vetor de até N itens |
| Bomba | posição, velocidade de queda, ativo ou não | no mesmo vetor das frutas, com um campo `tipo` |
| Estado do jogo | pontos, vidas, tempo restante, tela atual | 1 |

> Na implementação final não foi preciso o campo "ativo": o `filter` (`removeItensMortos`) compacta o vetor e o contador `numItens` diz quantos itens estão em uso.

### Onde entram os padrões funcionais

| Padrão | Uso no jogo |
|---|---|
| **map** | A cada quadro, aplicar a queda a todos os itens: `pos.y += vel * dt` |
| **filter** | Remover do vetor os itens coletados ou que saíram da tela (compactar os ativos) |
| **fold** | Somar os pontos das frutas coletadas no quadro e contar as bombas atingidas |

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
