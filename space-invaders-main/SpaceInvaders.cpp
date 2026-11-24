
#include <raylib.h>
#include "enemymanager.hpp"
#include <vector>

#define QTD_ESTRELAS 100

// ==========================================
// ESTADOS DO JOGO
// ==========================================

enum EstadoJogo {
    MENU,
    JOGANDO,
    GAME_OVER,
    VITORIA
};

EstadoJogo estado = MENU;

// ==========================================
// VARIAVEIS GLOBAIS
// ==========================================

// Estrelas
Vector2 estrelas[QTD_ESTRELAS];
float velocEstrelas[QTD_ESTRELAS];

// Nave
int naveX = 400;
int naveY = 700;

// Tiros do jogador
std::vector<Rectangle> tirosJogador;

float velocidadeTiro = 550.0f;
float intervaloDisparo = 0.25f;
float tempoDisparo = 0.0f;

// Pontuacao e vidas
int score = 0;
int vidas = 3;
int horda = 1;

// Protecao temporaria depois de sofrer dano
float tempoInvulnerabilidade = 0.0f;

// Inimigos
EnemyManager inimigos(4, 8, 800, 800);

// ==========================================
// ESTRELAS
// ==========================================

void criarEstrelas() {

    for (int i = 0; i < QTD_ESTRELAS; i++) {

        estrelas[i].x = GetRandomValue(0, 800);
        estrelas[i].y = GetRandomValue(0, 800);

        velocEstrelas[i] = GetRandomValue(2, 6);
    }
}

void movimentarEstrelas(float dt) {

    for (int i = 0; i < QTD_ESTRELAS; i++) {

        estrelas[i].y += velocEstrelas[i] * 60 * dt;

        if (estrelas[i].y > 800) {

            estrelas[i].y = 0;
            estrelas[i].x = GetRandomValue(0, 800);
        }
    }
}

void desenharEstrelas() {

    for (int i = 0; i < QTD_ESTRELAS; i++) {

        DrawCircle(
            (int)estrelas[i].x,
            (int)estrelas[i].y,
            2,
            WHITE
        );
    }
}

// ==========================================
// NAVE DO JOGADOR
// ==========================================

Rectangle GetNaveRect() {

    return Rectangle{
        (float)naveX - 20,
        (float)naveY - 20,
        40,
        40
    };
}

void desenharNave() {

    // Pisca enquanto estiver invulneravel
    if (tempoInvulnerabilidade > 0 &&
        ((int)(tempoInvulnerabilidade * 10) % 2 == 0)) {
        return;
    }

    DrawCircle(naveX, naveY, 20, WHITE);

    // Detalhe da nave
    DrawTriangle(
        Vector2{(float)naveX - 12, (float)naveY + 8},
        Vector2{(float)naveX, (float)naveY - 20},
        Vector2{(float)naveX + 12, (float)naveY + 8},
        SKYBLUE
    );
}

void movimentarNave(float dt) {

    float velocidade = 400.0f;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        naveX -= (int)(velocidade * dt);
    }

    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        naveX += (int)(velocidade * dt);
    }

    if (naveX < 20) {
        naveX = 20;
    }

    if (naveX > 780) {
        naveX = 780;
    }
}

// ==========================================
// TIROS DO JOGADOR
// ==========================================

void dispararJogador() {

    if (IsKeyDown(KEY_SPACE) && tempoDisparo <= 0) {

        Rectangle tiro = {
            (float)naveX - 3,
            (float)naveY - 25,
            6,
            18
        };

        tirosJogador.push_back(tiro);

        tempoDisparo = intervaloDisparo;
    }
}

void atualizarTirosJogador(float dt) {

    if (tempoDisparo > 0) {
        tempoDisparo -= dt;
    }

    for (int i = 0; i < (int)tirosJogador.size(); i++) {

        tirosJogador[i].y -= velocidadeTiro * dt;

        if (tirosJogador[i].y + tirosJogador[i].height < 0) {

            tirosJogador.erase(tirosJogador.begin() + i);
            i--;
        }
    }
}

void desenharTirosJogador() {

    for (auto& tiro : tirosJogador) {

        DrawRectangleRec(tiro, YELLOW);
    }
}

// ==========================================
// COLISAO DOS TIROS COM OS ALIENS
// ==========================================

void verificarColisoesTiros() {

    for (int t = 0; t < (int)tirosJogador.size(); t++) {

        bool tiroAcertou = false;

        for (auto& linha : inimigos.grid) {

            for (auto& alien : linha) {

                if (!alien.estaVivo) {
                    continue;
                }

                if (CheckCollisionRecs(
                    tirosJogador[t],
                    alien.GetRect()
                )) {

                    alien.estaVivo = false;

                    score += 100;

                    tiroAcertou = true;

                    break;
                }
            }

            if (tiroAcertou) {
                break;
            }
        }

        if (tiroAcertou) {

            tirosJogador.erase(tirosJogador.begin() + t);
            t--;
        }
    }
}

// ==========================================
// CONTAGEM DE ALIENS
// ==========================================

int contarAliensVivos() {

    int quantidade = 0;

    for (auto& linha : inimigos.grid) {

        for (auto& alien : linha) {

            if (alien.estaVivo) {
                quantidade++;
            }
        }
    }

    return quantidade;
}

// ==========================================
// SISTEMA DE VIDAS
// ==========================================

void receberDano() {

    if (tempoInvulnerabilidade > 0) {
        return;
    }

    vidas--;

    tempoInvulnerabilidade = 1.5f;

    if (vidas <= 0) {
        vidas = 0;
        estado = GAME_OVER;
    }
}

void verificarColisoesInimigos(float dt) {

    Rectangle naveRect = GetNaveRect();

    // Colisao dos tiros inimigos com o jogador
    for (int i = 0; i < (int)inimigos.tiros.size(); i++) {

        if (CheckCollisionRecs(
            naveRect,
            inimigos.tiros[i].rect
        )) {

            inimigos.tiros.erase(inimigos.tiros.begin() + i);

            receberDano();

            i--;

            if (estado == GAME_OVER) {
                return;
            }
        }
    }

    // Colisao fisica dos aliens com a nave
    for (auto& linha : inimigos.grid) {

        for (auto& alien : linha) {

            if (!alien.estaVivo) {
                continue;
            }

            Rectangle alienRect = alien.GetRect();

            if (CheckCollisionRecs(naveRect, alienRect)) {

                alien.estaVivo = false;

                receberDano();

                if (estado == GAME_OVER) {
                    return;
                }
            }

            // Alien chegou muito perto do jogador
            if (alien.GetRect().y + alien.GetRect().height >= naveY) {

                estado = GAME_OVER;
                return;
            }
        }
    }

    // Atualiza protecao temporaria
    if (tempoInvulnerabilidade > 0) {
        tempoInvulnerabilidade -= dt;
    }
}

// ==========================================
// INTERFACE DO JOGO
// ==========================================

void desenharInterface() {

    DrawText(
    TextFormat("HORDA: %d", horda),
    330,
    55,
    25,
    YELLOW
    );

    DrawText("SCORE", 30, 25, 25, GREEN);

    DrawText(
        TextFormat("%06d", score),
        30,
        55,
        30,
        WHITE
    );

    DrawText("VIDAS", 650, 25, 25, GREEN);

    for (int i = 0; i < vidas; i++) {

        DrawCircle(665 + i * 35, 75, 10, RED);
    }

    DrawText(
        TextFormat("ALIENS: %d", contarAliensVivos()),
        330,
        25,
        20,
        WHITE
    );
}

// ==========================================
// REINICIAR PARTIDA
// ==========================================

void configurarHorda() {

    // Aumenta a quantidade de linhas
    int linhas = 4 + (horda - 1);

    // Limite de 7 linhas
    if (linhas > 7) {
        linhas = 7;
    }

    int colunas = 8;

    // Aumenta a velocidade dos inimigos
    float velocidade = 100.0f + (horda - 1) * 25.0f;

    if (velocidade > 350.0f) {
        velocidade = 350.0f;
    }

    // Diminui o intervalo entre disparos
    float intervalo = 1.5f - (horda - 1) * 0.12f;

    if (intervalo < 0.45f) {
        intervalo = 0.45f;
    }

    // Aumenta a velocidade dos projeteis
    float velocidadeTiro = 250.0f + (horda - 1) * 30.0f;

    if (velocidadeTiro > 430.0f) {
        velocidadeTiro = 430.0f;
    }

    // Cria a nova formacao
    inimigos = EnemyManager(linhas, colunas, 800, 800);

    // Aplica a dificuldade
    inimigos.SetDifficulty(
        velocidade,
        intervalo,
        velocidadeTiro
    );
}

void avancarHorda() {

    horda++;

    // Limpa os tiros antigos
    tirosJogador.clear();

    // Protege o jogador durante a transicao
    tempoInvulnerabilidade = 2.0f;

    tempoDisparo = 0;

    // Cria a proxima horda
    configurarHorda();
}

void reiniciarJogo() {

    naveX = 400;
    naveY = 700;

    tirosJogador.clear();

    tempoDisparo = 0;
    tempoInvulnerabilidade = 0;

    score = 0;
    vidas = 3;

    horda = 1;

    configurarHorda();

    criarEstrelas();

    estado = JOGANDO;
}

// ==========================================
// MENU
// ==========================================

void desenharMenu() {

    DrawText(
        "SPACE INVADERS",
        200,
        180,
        45,
        GREEN
    );

    DrawText(
        "PRESSIONE ENTER PARA JOGAR",
        170,
        370,
        25,
        WHITE
    );

    DrawText(
        "A/D ou SETAS - MOVIMENTAR",
        230,
        450,
        20,
        LIGHTGRAY
    );

    DrawText(
        "ESPACO - ATIRAR",
        285,
        485,
        20,
        LIGHTGRAY
    );

    DrawText(
        "Jogo feito por: Raul, Felipe Cosmoski, Joao Paulo Dalzoto",
        65,
        750,
        18,
        WHITE
    );
}

// ==========================================
// GAME OVER
// ==========================================

void desenharGameOver() {

    DrawText(
        "GAME OVER",
        250,
        220,
        55,
        RED
    );

    DrawText(
        TextFormat("PONTUACAO FINAL: %d", score),
        250,
        330,
        25,
        WHITE
    );

    DrawText(
        "PRESSIONE R PARA REINICIAR",
        190,
        430,
        25,
        GREEN
    );

    DrawText(
        "PRESSIONE ESC PARA SAIR",
        220,
        480,
        20,
        LIGHTGRAY
    );
}

// ==========================================
// VITORIA
// ==========================================

void desenharVitoria() {

    DrawText(
        "VOCE VENCEU!",
        210,
        220,
        55,
        GREEN
    );

    DrawText(
        TextFormat("PONTUACAO FINAL: %d", score),
        250,
        330,
        25,
        WHITE
    );

    DrawText(
        "TODOS OS ALIENS FORAM DESTRUIDOS!",
        150,
        390,
        22,
        YELLOW
    );

    DrawText(
        "PRESSIONE R PARA JOGAR NOVAMENTE",
        150,
        470,
        23,
        GREEN
    );
}

// ==========================================
// MAIN
// ==========================================


int main() {

    InitWindow(800, 800, "Space Invaders");

    SetTargetFPS(60);

    criarEstrelas();

    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        // ATUALIZACAO DO JOGO

        if (estado == MENU) {

            if (IsKeyPressed(KEY_ENTER)) {
                reiniciarJogo();
            }
        }

        else if (estado == JOGANDO) {

            movimentarEstrelas(dt);
            movimentarNave(dt);
            dispararJogador();
            atualizarTirosJogador(dt);

            inimigos.Update(dt);

            verificarColisoesTiros();
            verificarColisoesInimigos(dt);

            // Avanca para a proxima horda
            // somente se o jogador ainda estiver vivo
            if (estado == JOGANDO && contarAliensVivos() == 0) {
                avancarHorda();
            }
        }

        else if (estado == GAME_OVER || estado == VITORIA) {

            if (IsKeyPressed(KEY_R)) {
                reiniciarJogo();
            }
        }

        // DESENHO

        BeginDrawing();

        ClearBackground(BLACK);

        desenharEstrelas();

        if (estado == MENU) {

            desenharMenu();
        }

        else if (estado == JOGANDO) {

            inimigos.Draw();

            desenharNave();

            desenharTirosJogador();

            desenharInterface();
        }

        else if (estado == GAME_OVER) {

            desenharGameOver();
        }

        else if (estado == VITORIA) {

            desenharVitoria();
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
