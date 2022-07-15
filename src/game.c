#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "core.h"

renderObject background;

typedef enum {empty, falling, fixed} cellState;
typedef enum {I, J, L, O, S, T, Z} tetrominoName;

typedef struct{
  tetrominoName name;
  SDL_Color color;
  int rotation[32]; //Using Sega Rotation system
}_tetroData;

_tetroData tetrominoData[7]={
  {I, {7,212,227, 255}, {-2,1,-1,0,0,-1,1,-2,2,-1,1,0,0,1,-1,2,-2,1,-1,0,0,-1,1,-2,2,-1,1,0,0,1,-1,2}},
  {J, {13,66,163, 255}, {0,-1,-1,0,0,1,1,2,2,-1,1,-2,0,-1,-1,0,0,2,1,1,0,0,-1,-1,-2,0,-1,1,0,0,1,-1}},
  {L, {163,78,13, 255}, {2,1,-1,0,0,1,1,2,0,1,1,-2,0,-1,-1,0,-2,0,1,1,0,0,-1,-1,0,-2,-1,1,0,0,1,-1}},
  {O, {232, 232, 21, 255}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
  {S, {59,201,12, 255}, {1,0,2,1,-1,0,0,1,-1,0,-2,-1,1,0,0,-1,1,0,2,1,-1,0,0,1,-1,0,-2,-1,1,0,0,-1}},
  {T, {145,15,171, 255}, {1,0,-1,0,0,1,1,2,1,0,1,-2,0,-1,-1,0,-1,1,1,1,0,0,-1,-1,-1,-1,-1,1,0,0,1,-1}},
  {Z, {201,22,25, 255}, {-2,1,-1,0,0,1,1,0,2,-1,1,0,0,-1,-1,0,-2,1,-1,0,0,1,1,0,2,-1,1,0,0,-1,-1,0}}
};

int currentTetronim;
int gameState = 1; //-1 is pause. +1 is playing. 2 is game over
int currentRotation = 0;

typedef struct{
  cellState state;
  renderObject cellRenderObject;
}cell;

int tetromino[7][8] =  {
  {0,0,0,0,1,1,1,1}, //I Block
  {1,0,0,0,1,1,1,0}, //J Block
  {0,0,1,0,1,1,1,0}, //L Block
  {0,1,1,0,0,1,1,0}, //O Block
  {0,1,1,0,1,1,0,0}, //S Block
  {0,1,0,0,1,1,1,0}, //T Block
  {1,1,0,0,0,1,1,0}  //Z Block
};

int fallingSpeed = 60;
int fastFallingSpeed = 5;
cell grid[10][20];
SDL_Point fallingCells[4];

int spawnList[7] = {0,1,2,3,4,5,6};
int listCurrent = 0;
bool canRotate = true;

int score = 0;

void newSpawnList(){
  int j, temp;
  listCurrent = 0;
  srand(time(0));
  for(int i = 0; i < 7; i++){
    j = rand() % 7;
    temp = spawnList[i];
    spawnList[i] = spawnList[j];
    spawnList[j] = temp;
  }
}

renderObject scoreText[10];
SDL_Texture *digitsTextures[11];
void LoadDigitsTextures(){
  int seperation = 22;
  SDL_Point startingPosition = newPoint(370, 470);
  digitsTextures[0] = textureFromImage("assets/zero.bmp");
  digitsTextures[1] = textureFromImage("assets/one.bmp");
  digitsTextures[2] = textureFromImage("assets/two.bmp");
  digitsTextures[3] = textureFromImage("assets/three.bmp");
  digitsTextures[4] = textureFromImage("assets/four.bmp");
  digitsTextures[5] = textureFromImage("assets/five.bmp");
  digitsTextures[6] = textureFromImage("assets/six.bmp");
  digitsTextures[7] = textureFromImage("assets/seven.bmp");
  digitsTextures[8] = textureFromImage("assets/eight.bmp");
  digitsTextures[9] = textureFromImage("assets/nine.bmp");
  digitsTextures[10] = textureFromImage("assets/empty.bmp");
  for(int i = 0; i < 10; i++){
    scoreText[i] = newRenderObject("");
    scoreText[i].texture = digitsTextures[i];
    scoreText[i].position = addPoints(startingPosition, newPoint(seperation * i, 0));
    Draw(&scoreText[i]);
  }
}

void UpdateScoreText(int _score){
  //Do text stuff here
  int lenght, temp = 10;

  //Clear the scores text
  for(int i = 0; i< 10; i++)
    scoreText[i].texture = digitsTextures[10];
  //Find the number of digits
  lenght = (_score == 0)?1:SDL_log10(_score)+1;
  //Update textures to show score
  for(int i = lenght -1, j = 0; i > -1; i--, j++){
    if(j == 0)
      scoreText[i].texture = digitsTextures[_score % 10];
    else{
      scoreText[i].texture = digitsTextures[(_score / temp) % 10];
      temp *= 10;
    }
  }
}

void calculateScore(){
  int completeRows[4], noOfRows = 0;
  int noOfCells;
  for(int y = 19; y > -1; y--){
    noOfCells = 0;
    for(int x = 0; x < 10; x++){
      if (grid[x][y].state == fixed)
        noOfCells++;
    }
    if(noOfCells >= 10) completeRows[noOfRows++] = y;
    if(noOfCells == 0) break;
  }

  if (noOfRows == 1) score += 40;
  else if(noOfRows == 2) score += 100;
  else if(noOfRows == 3) score += 300;
  else if(noOfRows == 4) score += 1200;
  else return;

  printf("Score is %d\n", score);
  UpdateScoreText(score);
  for(int i = 0; i < noOfRows; i++){
    for(int x =0; x < 10; x++){
      grid[x][completeRows[i]].state = empty;
      SDL_SetTextureColorMod(grid[x][completeRows[i]].cellRenderObject.texture, 255, 255, 255);
    }
  }

  for(int i = noOfRows - 1; i > -1; i--){
    for(int y = completeRows[i] - 1; y > -1; y--){
      for(int x = 0; x < 10; x++){
        if(grid[x][y].state == fixed){
          cellState tempState;
          SDL_Texture* tempTexture;
          tempState = grid[x][y].state;
          grid[x][y].state = grid[x][y+1].state;
          grid[x][y+1].state = tempState;

          tempTexture = grid[x][y].cellRenderObject.texture;
          grid[x][y].cellRenderObject.texture = grid[x][y + 1].cellRenderObject.texture;
          grid[x][y + 1].cellRenderObject.texture = tempTexture;
        }
      }
    }
  }
}

void spawnTetromino(){
  int *toSpawnTetromino = tetromino[spawnList[listCurrent]];
  int z = 0, w = 0;
  currentTetronim = spawnList[listCurrent];
  currentRotation = 0;

  //Update Falling Cells list
  for(int y = 0; y < 2; y++){
    for(int x = 3; x < 7; x++){
      if(grid[x][y].state == fixed)
        gameState = 2;
      grid[x][y].state = toSpawnTetromino[z++];
      if(grid[x][y].state == falling){
        fallingCells[w++] = newPoint(x, y);
  SDL_SetTextureColorMod(grid[x][y].cellRenderObject.texture , tetrominoData[currentTetronim].color.r, tetrominoData[currentTetronim].color.g, tetrominoData[currentTetronim].color.b);
      }
    }
  }

  listCurrent++;
  if(listCurrent > 6){
    newSpawnList();
  }
}

void getNewTetromino(){
  for(int i = 0; i < 4; i ++){
    grid[fallingCells[i].x][fallingCells[i].y].state = fixed;
  }
  calculateScore();
  spawnTetromino();
}

void Rotate(){
  //Code here
  SDL_Color currentColor = tetrominoData[currentTetronim].color;
  int* rotationData = tetrominoData[currentTetronim].rotation;
  int cellX, cellY;
  int _tempRotation = currentRotation;
  if(++_tempRotation > 3) _tempRotation = 0;
  rotationData += _tempRotation * 8;
  //Check if there is space to rotate
  for(int i = 0; i < 4; i++){
    cellX = rotationData[i * 2];
    cellY = rotationData[i * 2 + 1];
    if(grid[fallingCells[i].x + cellX][fallingCells[i].y + cellY].state == fixed || (fallingCells[i].y + cellY) > 19 || (fallingCells[i].y + cellY) < 0 || (fallingCells[i].x + cellX) > 9 || (fallingCells[i].x + cellX) < 0 )
      return;
  }

  currentRotation = _tempRotation;

  for(int i = 0; i < 4; i++){
    cellX = rotationData[i * 2];
    cellY = rotationData[i * 2 + 1];
    grid[fallingCells[i].x][fallingCells[i].y].state = empty;
    SDL_SetTextureColorMod(grid[fallingCells[i].x][fallingCells[i].y].cellRenderObject.texture, 255, 255, 255);
  }
  for(int i = 0; i < 4; i++){
    cellX = rotationData[i * 2];
    cellY = rotationData[i * 2 + 1];
    grid[fallingCells[i].x + cellX][fallingCells[i].y + cellY].state = falling;
    SDL_SetTextureColorMod(grid[fallingCells[i].x + cellX][fallingCells[i].y + cellY].cellRenderObject.texture,tetrominoData[currentTetronim].color.r,tetrominoData[currentTetronim].color.g,tetrominoData[currentTetronim].color.b);
    fallingCells[i] = addPoints(fallingCells[i], newPoint(cellX,cellY));
  }
}

void moveTetrominoDown(){
  canRotate = false;
  //Check if there is space to move down the tetromino
  for(int i =0 ;i < 4; i++){
    if(grid[fallingCells[i].x][fallingCells[i].y + 1].state == fixed || (fallingCells[i].y + 1) > 19){
      getNewTetromino();
      canRotate = true;
      return;
    }
  }

  //Move the tetromino down
  for(int i = 0; i < 4; i++){
    grid[fallingCells[i].x][fallingCells[i].y].state = empty;
    SDL_SetTextureColorMod(grid[fallingCells[i].x][fallingCells[i].y].cellRenderObject.texture, 255, 255, 255);
  }

  for(int i = 0; i < 4; i++){
    grid[fallingCells[i].x][fallingCells[i].y + 1].state = falling;
    SDL_SetTextureColorMod(grid[fallingCells[i].x][fallingCells[i].y + 1].cellRenderObject.texture,tetrominoData[currentTetronim].color.r,tetrominoData[currentTetronim].color.g,tetrominoData[currentTetronim].color.b);
    fallingCells[i] = addPoints(fallingCells[i], newPoint(0,1));
  }
  canRotate = true;
}

void moveTetromino(){
  int input = KeyIsDown(SDLK_d) - KeyIsDown(SDLK_a);
  for(int i =0; i<4; i++){
    if(grid[fallingCells[i].x + input][fallingCells[i].y].state == fixed || (fallingCells[i].x + input) < 0 || (fallingCells[i].x + input) > 9){
      return;
    }
  }

  for(int i = 0; i < 4; i++){
    grid[fallingCells[i].x][fallingCells[i].y].state = empty;
    SDL_SetTextureColorMod(grid[fallingCells[i].x][fallingCells[i].y].cellRenderObject.texture, 255, 255, 255);
  }

  for(int i = 0; i < 4; i++){
    grid[fallingCells[i].x + input][fallingCells[i].y].state = falling;
    SDL_SetTextureColorMod(grid[fallingCells[i].x + input][fallingCells[i].y].cellRenderObject.texture,tetrominoData[currentTetronim].color.r,tetrominoData[currentTetronim].color.g,tetrominoData[currentTetronim].color.b);
    fallingCells[i] = addPoints(fallingCells[i], newPoint(input,0));
  }
}

void OnStart(){
  background = newRenderObject("assets/Background.bmp");
  Draw(&background);
  SetWindow(1244, 700, 120, "Tetris");
  newSpawnList();
  for(int x = 0; x < 10; x++){
    for(int y = 0; y < 20; y++){
      grid[x][y].state = empty;
      grid[x][y].cellRenderObject = newRenderObject("assets/cell.bmp");
      grid[x][y].cellRenderObject.scale = newVector(0.35,0.35);
      grid[x][y].cellRenderObject.position = newPoint(35 * x, 35 * y);
      Draw(&grid[x][y].cellRenderObject);
    }
  }
  LoadDigitsTextures();
  UpdateScoreText(0);
  spawnTetromino();
}

void Restart(){
  score = 0;
  newSpawnList();
  for(int x = 0; x < 10; x++){
    for(int y = 0; y < 20; y++){
      grid[x][y].state = empty;
      SDL_SetTextureColorMod(grid[x][y].cellRenderObject.texture, 255, 255, 255);
    }
  }
  UpdateScoreText(0);
  spawnTetromino();
  gameState = 1;
}

int _count = 0;
int _currentSpeed;
void PerFrameUpdate(){
  if(KeyIsUp(SDLK_p))
    gameState *= -1;

  if(KeyIsUp(SDLK_r))
    Restart();

  if(gameState == 1){
    if(_count > _currentSpeed){
      moveTetrominoDown();
      _count = 0;
    }

    if(KeyIsUp(SDLK_w) && canRotate)
      Rotate();

    if(KeyIsPressed(SDLK_s))
      _currentSpeed = fastFallingSpeed;
    else
      _currentSpeed = fallingSpeed;
     moveTetromino();
    _count++;
  }
}
