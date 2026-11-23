#pragma once
#include <raylib.h>

class Alien{
    public:
        bool estaVivo;
        Alien(Vector2 posicao, Vector2 tam, bool vivo = true);
        void Draw();
        Rectangle GetRect();
        void Mover(Vector2 deslocamento);

    private:
    Vector2 pos;
    Vector2 tamanho;
};