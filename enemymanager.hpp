#pragma once
#include <raylib.h>
#include <vector>
#include "alien.hpp"

struct TiroInimigo{
    Rectangle rect;
    float velocidadeY;
};

class EnemyManager{
    public:
        std::vector<std::vector<Alien>> grid;
        std::vector<TiroInimigo> tiros;
        EnemyManager(int linhas, int colunas, int larguraTela, int alturaTela);
        void Update(float dt);
        void Draw();
    
    private:
        int larguraTela;
        int alturaTela;
        float velocidadeX;
        int direcaoX;
        float passoDescida;
        float timerTiro;
        float intervaloTiro;
        void CriarGrid(int linhas, int colunas);
        bool BateuNaBorda();
        void InverterEDescer();
        void Atirar();
        void AtualizarTiros(float dt);
};