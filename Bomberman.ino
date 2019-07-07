//Inclusão da biblioteca U8glib
#include "U8glib.h"

//Pinagem  do display Nokia 5110 (CLK=7, DIN=8, CE=10, DC=9, RST=11)
U8GLIB_PCD8544 u8g(7, 8, 10, 9, 11);

//Seleção de tela [0 - Início, 1 - Jogo, 2 - Fim]
int tela = 0;

//Leitura dos botões
int analogPin = A3;
int val = 0;

//Coordenadas do personagem e da bomba (só aceita uma bomba por enquanto)
int personagem_x=0, personagem_y=0, inimigo_x = -1, inimigo_y = -1, bomba_x=-1, bomba_y = -1;

//Nivel atual
int nivel = 1;

//Variáveis de delay (é o mesmo que usar delay(), mas sem congelar o programa inteiro)
unsigned long delayPersonagem = 0, delayInimigo = 0, delayBomba = 0, delayExplosao = 0;
bool delayPersonagem_r = false, delayInimigo_r = false, delayBomba_r = false, delayExplosao_r = false;

//Permite que o texto fique alternando no inicio e fim
bool mostrarTexto = true;

//Matriz que armazena o mapa
int mapa[6][10];

//Sprites (cada matriz é um sprite/desenho)
const unsigned char personagem[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {1, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

const unsigned char inimigo[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 0, 1, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

const unsigned char tijolo[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 1, 0, 1, 0},
    {0, 1, 0, 0, 1, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 0, 1, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

const unsigned char bomba[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

const unsigned char explosao[5][8][8] = {
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 1},
    {0, 0, 1, 0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 1, 0},
    {0, 1, 0, 0, 1, 0, 0, 1}
},

  {
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 1, 1, 0, 0, 0},
      {0, 0, 1, 0, 0, 1, 0, 0},
      {0, 1, 0, 0, 0, 0, 1, 0},
      {0, 0, 0, 1, 1, 0, 0, 0},
      {0, 0, 1, 0, 0, 1, 0, 0},
      {0, 1, 0, 0, 0, 0, 1, 0},
      {0, 0, 0, 0, 0, 0, 0, 0}
  },
  
  {
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 1, 0, 0, 1, 0, 0, 0},
      {0, 0, 1, 0, 0, 1, 0, 0},
      {0, 0, 0, 1, 0, 0, 1, 0},
      {0, 0, 0, 1, 0, 0, 1, 0},
      {0, 0, 1, 0, 0, 1, 0, 0},
      {0, 1, 0, 0, 1, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0}
  },
  
  {
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 1, 0, 0, 0, 0, 1, 0},
      {0, 0, 1, 0, 0, 1, 0, 0},
      {0, 0, 0, 1, 1, 0, 0, 0},
      {0, 1, 0, 0, 0, 0, 1, 0},
      {0, 0, 1, 0, 0, 1, 0, 0},
      {0, 0, 0, 1, 1, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0}
  },
  
  {
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 1, 0, 0, 1, 0},
      {0, 0, 1, 0, 0, 1, 0, 0},
      {0, 1, 0, 0, 1, 0, 0, 0},
      {0, 1, 0, 0, 1, 0, 0, 0},
      {0, 0, 1, 0, 0, 1, 0, 0},
      {0, 0, 0, 1, 0, 0, 1, 0},
      {0, 0, 0, 0, 0, 0, 0, 0}
  }

};

const unsigned char portal[8][8] = {
    {0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {1, 1, 0, 0, 0, 0, 1, 0},
    {1, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 1},
    {0, 1, 0, 0, 0, 0, 1, 1},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0}
};

//Telas
void Reset();       //Função que reinicia o mapa do jogo
void Tela_Inicio(); //Função que desenha a tela de início
void Tela_Nivel();  //Função que desenha a tela de nível
void Tela_Jogo();   //Função que desenha a tela de jogo (mapa, personagens e bombas)
void Tela_Fim();    //Função que desenha a tela de fim de jogo

//Animações
void ExplodirBomba();   //Explosão da bomba
void MoverPersonagem(); //Movimentação do personagem
void MoverInimigo();    //Movimentação do inimigo

//Desenhos
void DesenharMapa();
void DesenharSprite(unsigned char sprite[8][8], int x, int y);

void setup(){

  Serial.begin(9600);         //Inicia a leitura da porta (leitura dos botões analógicos)
  randomSeed(analogRead(0));  //Define a seed para randomização
  delayPersonagem = millis(); //Inicia o delay do personagem (neste caso é utilizado para que o texto pisque na tela de inicio)
  
}

void loop(){

  u8g.firstPage(); //Início do loop que desenha os frames do jogo
  do{

    //Seleção de tela
    switch(tela){

      case 0:
        Tela_Inicio();
      break;

      case 1:
        Tela_Nivel();
      break;

      case 2:
        Tela_Jogo();
      break;

      case 3:
        Tela_Fim();
      break;
      
    }

  } while(u8g.nextPage()); //Fim do loop que desenha os frames
  
}

void ExplodirBomba(){

  //Cada 'if' realiza a explosão para um lado, verificando se é possível avançar para o lado, explodindo os blocos e derrotando os inimigos;
  //No mapa, os números 10~14 representam a animação de explosão da bomba

  if(bomba_y > 0){
    if(mapa[bomba_y-1][bomba_x] == 2) mapa[bomba_y-1][bomba_x] = 0;
    else if(mapa[bomba_y-1][bomba_x] == 0) mapa[bomba_y-1][bomba_x] = 11;
    
    if(bomba_x == inimigo_x && bomba_y-1 == inimigo_y){
      inimigo_x = -1;
      inimigo_y = -1;  
    }
  }
  
  if(bomba_y < 5){
    if(mapa[bomba_y+1][bomba_x] == 2) mapa[bomba_y+1][bomba_x] = 0;
    else if(mapa[bomba_y+1][bomba_x] == 0) mapa[bomba_y+1][bomba_x] = 13;
    
    if(bomba_x == inimigo_x && bomba_y+1 == inimigo_y){
      inimigo_x = -1;
      inimigo_y = -1;  
    }
  }
  
  if(bomba_x > 0){
    if(mapa[bomba_y][bomba_x-1] == 2) mapa[bomba_y][bomba_x-1] = 0;
    else if(mapa[bomba_y][bomba_x-1] == 0) mapa[bomba_y][bomba_x-1] = 14;
    
    if(bomba_x-1 == inimigo_x && bomba_y == inimigo_y){
      inimigo_x = -1;
      inimigo_y = -1;  
    }
  }
  
  if(bomba_x < 9){
    if(mapa[bomba_y][bomba_x+1] == 2) mapa[bomba_y][bomba_x+1] = 0;
    else if(mapa[bomba_y][bomba_x+1] == 0) mapa[bomba_y][bomba_x+1] = 12;
    
    if(bomba_x+1 == inimigo_x && bomba_y == inimigo_y){
      inimigo_x = -1;
      inimigo_y = -1;  
    }
  }

  //Define que o bloco em que a bomba estava terá uma animação de explosão
  mapa[bomba_y][bomba_x] = 10;

  //Inicia o delay da animação de explosão da bomba
  delayExplosao = millis();
  delayExplosao_r = true;

  //Remove a bomba do mapa
  bomba_x = -1;
  bomba_y = -1;

}

void MoverPersonagem(){
  
  val = analogRead(analogPin);  //Lê o botão que está sendo pressionado

  //Se o personagem puder se mover, o programa verifica qual botão foi pressionado e se o personagem pode ir para esta direção
  //Se o personagem se mover, o delay de movimentação é acionado para que ele possa se mover somente a cada 0.5 segundo
  
  if(!delayPersonagem_r){
    if((val >= 20 && val <= 30) && (personagem_y > 0 && mapa[personagem_y-1][personagem_x] != 1 && mapa[personagem_y-1][personagem_x] != 2)){
      personagem_y -= 1;
      
      delayPersonagem = millis();
      delayPersonagem_r = true;
    }
      
    if((val >= 0 && val <= 10) && (personagem_x > 0 && mapa[personagem_y][personagem_x-1] != 1 && mapa[personagem_y][personagem_x-1] != 2)){
      personagem_x -= 1;

      delayPersonagem = millis();
      delayPersonagem_r = true;
    }
      
    if((val >= 50 && val <= 60) && (personagem_y < 5 && mapa[personagem_y+1][personagem_x] != 1 && mapa[personagem_y+1][personagem_x] != 2)){
      personagem_y += 1;

      delayPersonagem = millis();
      delayPersonagem_r = true;
    }
    
    if((val >= 100 && val <= 110) && (personagem_x < 9 && mapa[personagem_y][personagem_x+1] != 1 && mapa[personagem_y][personagem_x+1] != 2)){
      personagem_x += 1;

      delayPersonagem = millis();
      delayPersonagem_r = true;
    }

    //Se o jogador pressionar o botão de bomba, o programa verifica se ela já existe, caso não exista, ela recebe as coordenadas do personagem e aciona seu delay
    //O delay da bomba permite que ela seja explodida somente após 3 segundos   
    if((val >= 220 && val <= 230) && (bomba_x == -1 && bomba_y == -1)){
      bomba_x = personagem_x;
      bomba_y = personagem_y;

      delayBomba = millis();
      delayBomba_r = true;
    }
  }
  //Verifica se o delay de movimentação do personagem já acabou
  else{
    if((millis() - delayPersonagem) >= 500)
      delayPersonagem_r = false;
  }

  //Verifica se o personagem foi pego pela explosão da bomba
  if(mapa[personagem_y][personagem_x] >= 10 && mapa[personagem_y][personagem_x] < 15)
    Morrer();
      
}

void MoverInimigo(){

  //Verifica se o inimigo tocou o personagem (caso sim, o personagem morre)
  if(inimigo_x == personagem_x && inimigo_y == personagem_y)
      Morrer();
      
  //Verifica se o inimigo foi pego pela explosão da bomba (caso sim, o inimigo morre)
  else if(mapa[inimigo_y][inimigo_x] >= 10 && mapa[inimigo_y][inimigo_x] < 15){
    inimigo_x = -1;
    inimigo_y = -1;
  }
    
  //Se o inimigo puder se mover, é gerado um número aleatório que define para qual lado ele irá se mover
  //Se o lado selecionado estiver livre, o inimigo se move e aciona o delay de movimentação
  if(!delayInimigo_r){
    int direcao = random(0, 4);
    
    switch(direcao){
      
      case 0:
          if(inimigo_y > 0 && mapa[inimigo_y-1][inimigo_x] != 1 && mapa[inimigo_y-1][inimigo_x] != 2 && (inimigo_y-1 != bomba_y || inimigo_x != bomba_x)){
            inimigo_y--;

            delayInimigo = millis();
            delayInimigo_r = true;
          }
      break;
  
      case 1:
        if(inimigo_x < 9 && mapa[inimigo_y][inimigo_x+1] != 1 && mapa[inimigo_y][inimigo_x+1] != 2 && (inimigo_y != bomba_y || inimigo_x+1 != bomba_x)){
          inimigo_x++;

          delayInimigo = millis();
          delayInimigo_r = true;
        }
      break;
  
      case 2:
        if(inimigo_y < 5 && mapa[inimigo_y+1][inimigo_x] != 1 && mapa[inimigo_y+1][inimigo_x] != 2 && (inimigo_y+1 != bomba_y || inimigo_x != bomba_x)){
          inimigo_y++;

          delayInimigo = millis();
          delayInimigo_r = true;
        }
      break;
  
      case 3:
        if(inimigo_x > 0 && mapa[inimigo_y][inimigo_x-1] != 1 && mapa[inimigo_y][inimigo_x-1] != 2 && (inimigo_y != bomba_y || inimigo_x-1 != bomba_x)){
          inimigo_x--;

          delayInimigo = millis();
          delayInimigo_r = true;
        }
      break;
        
    }

    //Verifica novamente se o inimigo tocou o personagem ou a explosão da bomba
    if(inimigo_x == personagem_x && inimigo_y == personagem_y)
      Morrer();
    else if(mapa[inimigo_y][inimigo_x] >= 10 && mapa[inimigo_y][inimigo_x] < 15){
      inimigo_x = -1;
      inimigo_y = -1;
    }
    
  }
  //Verifica se o delay de movimentação do inimigo já acabou
  else{
    if((millis() - delayInimigo) >= 1000)
      delayInimigo_r = false;
  }
  
}

void Morrer(){

  //Reinicia o delay da bomba (utilizado para  exista um tempo mínimo de mudança entre as telas)
  delayBomba = millis();
  delayBomba_r = true;

  //Reinicia o nível atual
  nivel = 1;

  //Redireciona o jogador para a tela de fim de jogo
  tela = 3;
  
}

void Reset(){

  //
  //Esta função reinicia todas as variáveis do jogo, para que um novo mapa possa ser gerado
  //

  //Leitura dos botões
  analogPin = A3;
  val = 0;
  
  //Coordenadas do personagem e da bomba (só aceita uma bomba por enquanto)
  personagem_x  = 0;
  personagem_y  = 0;
  inimigo_x     = -1;
  inimigo_y     = -1;
  bomba_x       = -1;
  bomba_y       = -1;
  
  //Variáveis de delay (é o mesmo que usar delay(), mas sem congelar o programa inteiro)
  delayPersonagem   = millis();
  delayInimigo      = millis();
  delayBomba        = 0;
  delayExplosao     = 0;
  
  delayPersonagem_r = true;
  delayInimigo_r    = true;
  delayBomba_r      = false;
  delayExplosao_r   = false;

  //Variável que define se o texto deve ser mostrado ou não (telas de início e fim)
  mostrarTexto = true;

  //Loops que geram um novo mapa (definem os blocos vazios e fixos)
  int i, j;
  for(i=0;i<6;i++){
    for(j=0;j<10;j++){
      if(i > 0 && j > 0 && ((i+j) % 2 == 0) && i%2 != 0) mapa[i][j] = 1;
      else mapa[i][j] = 0;
    }
  }

  //Loops que geram um novo mapa (definem a posição do inimigo e dos tijolos)
  for(i=0;i<6;i++){
    for(j=0;j<10;j++){
      if(mapa[i][j] == 0 && (i > 1 || j > 1) && (inimigo_x+1 != j || inimigo_y != i) && (inimigo_x != j || inimigo_y+1 != i)){
        if(inimigo_x == -1 && inimigo_y == -1 && i > 0 && j > 0 && random(0, 10) > 6){ 
          inimigo_x = j;
          inimigo_y = i;
        }
        else mapa[i][j] = (random(0, 10) > 2 ? 2 : 0);  
      }
    }
  } 
  
}

void Tela_Inicio(){

  //Verifica se o delay do texto piscando terminou
  if((millis() - delayPersonagem) >= 750){
      delayPersonagem = millis();

      //Altera a situação do texto (define se ele pode ou não ser exibido)
      mostrarTexto = !mostrarTexto;
  }

  u8g.setFont(u8g_font_6x10);         //Define a fonte a ser utilizada nos textos
  u8g.drawRFrame(10, 8, 65, 15, 3);   //Desenha um quadrado com bordas arredondadas
  u8g.drawStr(13, 19, "Bomberman!");  //Desenha o texto: 'Bomberman!'

  //Caso o texto possa ser exibido, define a fonte e desenha o texto 'Press any button to start'
  if(mostrarTexto){
    u8g.setFont(u8g_font_4x6);
    u8g.drawStr(11, 35, "Press any button");
    u8g.drawStr(27, 42, "to start");
  }

  //Lê o botão que está sendo pressionado
  val = analogRead(analogPin);

  //Caso o jogador tenha apertado qualquer botão, ele é redirecionado para a tela de nível
  if((val >= 20 && val <= 30) || (val >= 220 && val <= 230) || (val >= 0 && val <= 10) || (val >= 50 && val <= 60) || (val >= 100 && val <= 110)){
    delayBomba = millis();
    delayBomba_r = true;

    tela = 1;
  }
  
}

void Tela_Nivel(){

  //
  //Escreve o nível atual e, após 2.5 segundos, redireciona o jogador para a tela de jogo
  //
  
  u8g.setFont(u8g_font_6x10);
  u8g.drawStr(22, 25, "Level ");

  u8g.setPrintPos(57, 25);  // (x,y)
  u8g.print(nivel);  //Value to print

  if(millis() - delayBomba >= 2500){
    tela = 2;
    Reset();
  }
  
}

void Tela_Jogo(){

  MoverPersonagem();  //Executa a função de movimentação do personagem
  DesenharMapa();     //Executa a função que desenha o mapa 
  DesenharSprite(personagem, personagem_x, personagem_y); //Executa a função que desenha o personagem

  //Caso a bomba exista, desenha ela
  if(bomba_x != -1 && bomba_y != -1){
    DesenharSprite(bomba, bomba_x, bomba_y);

    //Se a bomba já existir há 3 segundos, ela explode
    if((millis() - delayBomba) >= 3000){
      delayBomba_r = false;
      ExplodirBomba();
    }
  }

  //Se o inimigo existir, ele é desenhado e movimentado
  if(inimigo_x != -1 && inimigo_y != -1){
    DesenharSprite(inimigo, inimigo_x, inimigo_y);
    MoverInimigo();
  }
    
}

void Tela_Fim(){

  //Permite que o texto pisque a cada 750ms
  if((millis() - delayPersonagem) >= 750){
      delayPersonagem = millis();
      mostrarTexto = !mostrarTexto;
  }

  u8g.setFont(u8g_font_6x10);         //Define a fonte a ser utilizada nos textos
  u8g.drawRFrame(10, 8, 65, 15, 3);   //Desenha um retângulo com bordas arredondadas
  u8g.drawStr(13, 19, "Game over!");  //Desenha o texto: 'Game over'

  //Caso o texto possa ser exibido, desenha a frase: 'Press any button to continue'
  if(mostrarTexto){
    u8g.setFont(u8g_font_4x6);
    u8g.drawStr(11, 35, "Press any button");
    u8g.drawStr(22, 42, "to continue");
  }

  //Lê o botão que está sendo pressionado pelo jogador
  val = analogRead(analogPin);

  //Se o jogador pressionar qualquer botão, um novo mapa é gerado e ele é redirecionado para a tela de jogo
  if((millis() - delayBomba) >= 1000 && ((val >= 20 && val <= 30) || (val >= 220 && val <= 230) || (val >= 0 && val <= 10) || (val >= 50 && val <= 60) || (val >= 100 && val <= 110))){
    Reset();
    tela = 1;
  }
  
}

void DesenharMapa(){

  //Verifica se o delay da explosão da bomba terminou
  if((millis() - delayExplosao) >= 1500)
    delayExplosao_r = false;

  //Declaração de variáveis auxiliares
  int i, j;
  bool aux = false;

  //Loops que desenham o mapa
  for(i=0;i<6;i++){
    for(j=0;j<10;j++){
      
      //Caso a coordenada possua o valor 1, desenha um bloco fixo
      if(mapa[i][j] == 1)
        u8g.drawBox(j*8, i*8, 8, 8);

      //Caso a coordenada possua o valor 2, desenha um tijolo
      else if(mapa[i][j] == 2){
        DesenharSprite(tijolo, j, i);
        aux = true;
      }

      //Caso a coordenada possua o valor 5, desenha um portal
      else if(mapa[i][j] == 5){
        DesenharSprite(portal, j, i);

        //Se o personagem estiver no portal, o nível é encerrado
        if(personagem_x == j && personagem_y == i){ 
          delayBomba = millis();
          delayBomba_r = true;

          nivel++;
          tela = 1;
        }
        
      }

      //Caso a coordenada possua um valor entre 10 e 14, desenha a explosão da bomba
      else if(mapa[i][j] >= 10 && mapa[i][j] < 15){
        if(!delayExplosao_r)
          mapa[i][j] = 0;
        else
          DesenharSprite(explosao[mapa[i][j]-10], j, i);
      }
      
    }
  }

  //Inicia o portal caso o inimigo tenha sido derrotado e não exista mais nenhum tijolo
  if(!aux && inimigo_x == -1 && inimigo_y == -1)
    mapa[5][9] = 5;
    
}

void DesenharSprite(unsigned char sprite[8][8], int x, int y){

  //Variáveis auxiliares
  int i, j;

  //Loops que percorrem a matriz 8x8 do sprite
  for(i=0;i<8;i++)
    for(j=0;j<8;j++)
      if(sprite[i][j] == 1) //Caso o valor seja 1, desenha um pixel
        u8g.drawBox(j+(8*x), i+(8*y), 1, 1);
  
}
