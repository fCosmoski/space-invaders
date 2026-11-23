#include "alien.hpp"

Alien::Alien(Vector2 posicao, Vector2 tam, bool vivo){
    pos = posicao;
    tamanho = tam;
    estaVivo = vivo;
}

void Alien::Draw(){
    if(estaVivo == true){
        DrawRectangleRec(GetRect(), GREEN);
    }
}

Rectangle Alien::GetRect(){
    return Rectangle{pos.x, pos.y, tamanho.x, tamanho.y};
}

void Alien::Mover(Vector2 deslocamento){
    pos.x += deslocamento.x;
    pos.y += deslocamento.y;
}