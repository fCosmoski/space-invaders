#include <raylib.h>
#define QTD_ESTRELAS 100

//Variaveis universais
Vector2 estrelas[QTD_ESTRELAS];
float velocEstrelas[QTD_ESTRELAS];

//Funções
bool clicar(Vector2 mouse, Rectangle botao){
    if(CheckCollisionPointRec(mouse, botao)){
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            return true;
        }
    }
    return false;
}
void criarEstrelas(){

    for(int i = 0 ;i < QTD_ESTRELAS; i++){
        estrelas[i].x = GetRandomValue(0, 800);
        estrelas[i].y = GetRandomValue(0, 800);

        velocEstrelas[i] = GetRandomValue(2, 6);
    }
}
void movimentarEstrelas(){
    for(int i = 0; i < QTD_ESTRELAS; i++){
        estrelas[i].y += velocEstrelas[i];
        if(estrelas[i].y > 800){

        estrelas[i].y = 0;
        estrelas[i].x = GetRandomValue(0, 800);
        }
    }
}
void desenharEstrelas(){
    for(int i=0; i<QTD_ESTRELAS; i++){
        DrawCircle(estrelas[i].x, estrelas[i].y, 2, WHITE);
    }
}
void tempoScore(){
    static float tempo = 0;
    static int score = 0;

    tempo += GetFrameTime();
    if(tempo>=1.0f){
        score++;
        tempo-=1.0f;
    }
    DrawText(TextFormat("%d", score), 700, 80, 25, GREEN);
}
void nave(){
    static int naveX = 400;
    int naveY = 670;
    DrawCircle(naveX, naveY, 20, WHITE);
    if(IsKeyDown(KEY_A)){
        if(naveX != 0){
            naveX -= 8;
        }
    } else if (IsKeyDown(KEY_D)){
        if(naveX != 800){
            naveX += 8;
        }
    }    
}
int main()
{
    InitWindow(800, 800, "Space Invaders");
    SetTargetFPS(60);

    
    Rectangle botaoJogar = {340, 365, 120, 40};
    Rectangle botaoSair = {365, 445, 120, 40};
    Rectangle botaoteste = {320, 370, 120, 40};

    bool jogoIniciado = false;
    
    criarEstrelas();

    //Abre janela
    while(WindowShouldClose() == false){
        Vector2 mouse = GetMousePosition();

        if(clicar(mouse, botaoJogar)){
            jogoIniciado = true;
        }

        if(clicar(mouse, botaoSair)){
            break;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        //tela inicio
        if (jogoIniciado == false){

            DrawText("SPACE INVADERS", 250, 50, 30, GREEN);
            DrawText("JOGAR", 350, 370, 30, WHITE);
            DrawText("SAIR", 375, 450, 20, WHITE);
            DrawText("Jogo feito por: Raul, Felipe Cosmoski, João Paulo Dalzoto", 100, 750, 20, WHITE);
            
        } else {
            //Jogo começa:
            ClearBackground(BLACK);
            DrawText("Score", 650, 30, 35, GREEN);
            tempoScore();
            desenharEstrelas();
            nave();
            movimentarEstrelas();

            //Quando morrer no jogo
        }
        EndDrawing();

    }

    CloseWindow();
    return 0;
}
