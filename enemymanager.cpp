#include "enemymanager.hpp"

EnemyManager::EnemyManager(int linhas, int colunas, int largura, int altura) {
    larguraTela = largura;
    alturaTela = altura;
    velocidadeX = 100;
    direcaoX = 1;
    passoDescida = 20;
    timerTiro = 0;
    intervaloTiro = 1.5;
    CriarGrid(linhas, colunas);
}

void EnemyManager::CriarGrid(int linhas, int colunas){
    grid.clear();
    tiros.clear();
    float larguraAlien = 40;
    float alturaAlien = 28;
    float espacoX = 16;
    float espacoY = 16;
    float margemTopo = 70;
    float margemEsquerda = 60;
    for(int l=0; l<linhas; l++){
        std::vector<Alien> linhaAtual;
        for(int c=0; c<colunas; c++) {
            float x = margemEsquerda + c * (larguraAlien + espacoX);
            float y = margemTopo + l * (alturaAlien + espacoY);
            linhaAtual.push_back(Alien(Vector2{x, y}, Vector2{larguraAlien, alturaAlien}, true));
        }
        grid.push_back(linhaAtual);
    }
}

bool EnemyManager::BateuNaBorda(){
    for(auto& linha : grid){
        for(auto& alien : linha){
            if(!alien.estaVivo){
                continue;
            }
            Rectangle r = alien.GetRect();
            if(direcaoX>0 && r.x + r.width >= larguraTela){
                return true;
            }
            if(direcaoX<0 && r.x <= 0){
                return true;
            }
        }
    }
    return false;
}

void EnemyManager::InverterEDescer(){
    direcaoX *= -1;
    for(auto& linha : grid){
        for(auto& alien : linha){
            if(alien.estaVivo){
                alien.Mover(Vector2{0, passoDescida});
            }
        }
    }
}

void EnemyManager::Atirar(){
    std::vector<Alien*> vivos;
    for(auto& linha : grid){
        for(auto& alien : linha){
            if(alien.estaVivo){
                vivos.push_back(&alien);
            }
        }
    }
    if(vivos.empty()){
        return;
    }
    int sorteado = GetRandomValue(0, (int)vivos.size() - 1);
    Rectangle r = vivos[sorteado]->GetRect();
    TiroInimigo novoTiro;
    novoTiro.rect = Rectangle{r.x + r.width / 2 - 3, r.y + r.height, 6, 14};
    novoTiro.velocidadeY = 250;
    tiros.push_back(novoTiro);
}

void EnemyManager::AtualizarTiros(float dt){
    timerTiro += dt;
    if(timerTiro >= intervaloTiro){
        timerTiro = 0;
        Atirar();
    }
    for(size_t i=0; i<tiros.size(); i++){
        tiros[i].rect.y += tiros[i].velocidadeY * dt;
        if(tiros[i].rect.y > alturaTela){
            tiros.erase(tiros.begin() + i);
            i--;
        }
    }
}

void EnemyManager::Update(float dt){
    float deslocX = direcaoX * velocidadeX * dt;
    for(auto& linha : grid){
        for(auto& alien : linha){
            if(alien.estaVivo){
                alien.Mover(Vector2{deslocX, 0});
            }
        }
    }
    if(BateuNaBorda()){
        InverterEDescer();
    }
    AtualizarTiros(dt);
}

void EnemyManager::Draw(){
    for(auto& linha : grid){
        for(auto& alien : linha){
            alien.Draw();
        }
    }
    for(auto& tiro : tiros){
        DrawRectangleRec(tiro.rect, RED);
    }
}