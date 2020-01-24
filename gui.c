#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "gui.h"
#include "move.h"

int SCREEN_WIDTH;
int SCREEN_HEIGHT;

SDL_Window *gWindow = NULL;
SDL_Texture *gTexture = NULL;
SDL_Renderer *gRenderer = NULL;

int size[2] = {0};

void init()
{
  FILE * fptr = fopen("screen_settings.txt", "r");
  fscanf(fptr, "%d %d", &SCREEN_WIDTH, &SCREEN_HEIGHT);
  fclose(fptr);
  if(SDL_Init(SDL_INIT_VIDEO))
  {
    printf("Failed to initialize video!");
  }
  if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
  {
    printf("Failed to set hint!");
  }
  gWindow = SDL_CreateWindow("Bulltricker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if(gWindow == NULL)
  {
    printf("Failed to create window!");
  }
  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_TARGETTEXTURE);
  if(gRenderer == NULL)
  {
    printf("Failed to create renderer!");
  }
  SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
  if(!IMG_Init(IMG_INIT_PNG))
  {
    printf("Failed to initialize SDL_Image!");
  }
}

void close()
{
  SDL_DestroyRenderer(gRenderer);
  gRenderer = NULL;
  SDL_DestroyTexture(gTexture);
  gTexture = NULL;
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  IMG_Quit();
  SDL_Quit();
}

SDL_Texture *loadTexture(char *path)
{
  SDL_Texture *newTexture = NULL;
  newTexture = IMG_LoadTexture(gRenderer, path);
  if(newTexture == NULL)
  {
    printf("Failed to create texture from surface %s!\n%s", path, IMG_GetError());
  }
  return newTexture;
}

void loadGrid()
{
  SDL_Rect fillRect = {600, 0, 200, 580};
  gTexture = loadTexture("Sprites/Grid.png");
  SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
  SDL_DestroyTexture(gTexture);
  gTexture = NULL;
}

void fillBlank()
{
  SDL_RenderClear(gRenderer);
  SDL_Rect fillRect1 = {580, 0, 20, 580};
  SDL_Rect fillRect2 = {0, 580, 800, 20};
  SDL_Texture *newTexture = loadTexture("Sprites/fill1.png");
  SDL_RenderCopy(gRenderer, newTexture, NULL, &fillRect1);
  newTexture = loadTexture("Sprites/fill2.png");
  SDL_RenderCopy(gRenderer, newTexture, NULL, &fillRect2);
  SDL_DestroyTexture(newTexture);
  newTexture = NULL;
}

void draw(int **actual_matrix)
{
  fillBlank();
  loadGrid();
  int x, y;
  for(int i = 0, k = 0; i < 580, k < 15; k++)
  {
    for(int j = 0, l = 0; j < 580, l < 15; l++)
    {
      if(actual_matrix[l][k] == -1)
      {
        gTexture = loadTexture("Sprites/NP.png");
        SDL_Rect fillRect = {i, j, 20, 20};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=20;
        y=20;
      }
      else if(actual_matrix[l][k] == 0 && l % 2 == 1)
      {
        gTexture = loadTexture("Sprites/P0.png");
        SDL_Rect fillRect = {i, j, 20, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=20;
        y=60;
      }
      else if(actual_matrix[l][k] == 0 && l % 2 == 0)
      {
        gTexture = loadTexture("Sprites/P1.png");
        SDL_Rect fillRect = {i, j, 60, 20};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=20;
      }
      else if(actual_matrix[l][k] == -2)
      {
        gTexture = loadTexture("Sprites/PK.png");
        SDL_Rect fillRect = {i, j, 60, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=60;
      }
      else if(actual_matrix[l][k] == 13)
      {
        gTexture = loadTexture("Sprites/KB.png");
        SDL_Rect fillRect = {i, j, 60, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=60;
      }
      else if(actual_matrix[l][k] == 23)
      {
        gTexture = loadTexture("Sprites/KW.png");
        SDL_Rect fillRect = {i, j, 60, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=60;
      }
      else if(actual_matrix[l][k] == 121)
      {
        gTexture = loadTexture("Sprites/QBR.png");
        SDL_Rect fillRect = {i, j, 60, 20};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=20;
      }
      else if(actual_matrix[l][k] == 120)
      {
        gTexture = loadTexture("Sprites/QB.png");
        SDL_Rect fillRect = {i, j, 20, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=20;
        y=60;
      }
      else if(actual_matrix[l][k] == 221)
      {
        gTexture = loadTexture("Sprites/QWR.png");
        SDL_Rect fillRect = {i, j, 60, 20};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=20;
      }
      else if(actual_matrix[l][k] == 220)
      {
        gTexture = loadTexture("Sprites/QW.png");
        SDL_Rect fillRect = {i, j, 20, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=20;
        y=60;
      }
      else if(actual_matrix[l][k] == 110)
      {
        gTexture = loadTexture("Sprites/PBR.png");
        SDL_Rect fillRect = {i, j, 20, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=20;
        y=60;
      }
      else if(actual_matrix[l][k] == 210)
      {
        gTexture = loadTexture("Sprites/PWR.png");
        SDL_Rect fillRect = {i, j, 20, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=20;
        y=60;
      }
      else if(actual_matrix[l][k] == 111)
      {
        gTexture = loadTexture("Sprites/PB.png");
        SDL_Rect fillRect = {i, j, 60, 20};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=20;
      }
      else if(actual_matrix[l][k] == 211)
      {
        gTexture = loadTexture("Sprites/PW.png");
        SDL_Rect fillRect = {i, j, 60, 20};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=20;
      }
      j+=y;
      SDL_DestroyTexture(gTexture);
      gTexture = NULL;
    }
    i+=x;
  }
  SDL_RenderPresent(gRenderer);
}

void mainMenu(bool *quit)
{
  SDL_RenderClear(gRenderer);
  SDL_Rect fillRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  SDL_Texture *textureList[5] = {loadTexture("Sprites/Main_Menu_1.png"), loadTexture("Sprites/Main_Menu_2.png"), loadTexture("Sprites/Main_Menu_3.png"), loadTexture("Sprites/Main_Menu_4.png")};
  int i = 0;
  gTexture = textureList[i];
  SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
  SDL_RenderPresent(gRenderer);
  SDL_Event event;
  bool start = false;
  while(!start && !*quit)
  {
    if(SDL_WaitEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        *quit = true;
      }
      else if(event.type == SDL_KEYDOWN)
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_DOWN:
          {
            SDL_Delay(50);
            i++;
            i = i % 4;
            gTexture = textureList[i];
            SDL_RenderClear(gRenderer);
            SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
            SDL_RenderPresent(gRenderer);
            break;
          }
          case SDLK_UP:
          {
            SDL_Delay(50);
            i--;
            if(i == -1)
            {
              i = 3;
            }
            gTexture = textureList[i];
            SDL_RenderClear(gRenderer);
            SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
            SDL_RenderPresent(gRenderer);
            break;
          }
          case SDLK_RETURN:
          {
            SDL_Delay(150);
            if(i == 0)
            {
              start = true;
              SDL_DestroyTexture(gTexture);
              for(i = 0; i < 4; i++)
              {
                SDL_DestroyTexture(textureList[i]);
                textureList[i] = NULL;
              }
              gTexture = NULL;
              SDL_RenderClear(gRenderer);
            }
            break;
          }
        }
      }
    }
  }
}

void gameOver(int **actual_matrix, bool *game_over)
{
  if(check_mat(actual_matrix) == whites)
  {
    return;
  }
  else if(check_mat(actual_matrix) == blacks)
  {
    return;
  }
}

void getSize(int x, int y)
{
  if((y/20) % 4 == 0)
  {
    if((x/20) % 4 == 0)
    {
      size[0] = 20;
      size[1] = 20;
    }
    else
    {
      size[0] = 60;
      size[1] = 20;
    }
  }
  else
  {
    if((x/20) % 4 == 0)
    {
      size[0] = 20;
      size[1] = 60;
    }
    else
    {
      size[0] = 60;
      size[1] = 60;
    }
  }
}

int mapToInt(int x)
{
  int edges[15] = {20, 80, 100, 160, 180, 240, 260, 320, 340, 400, 420, 480, 500, 560, 580};
  for(int i = 0; i < 15; i++)
  {
    if(x < edges[i])
    {
      return i;
    }
  }
}

int mapToEdge(int x)
{
  int edges[15] = {0, 20, 80, 100, 160, 180, 240, 260, 320, 340, 400, 420, 480, 500, 560};
  return edges[x];
}

void setBlendMode(SDL_BlendMode blendMode)
{
  SDL_SetTextureBlendMode(gTexture, blendMode);
}

void setAlpha(unsigned char alpha)
{
  SDL_SetTextureAlphaMod(gTexture, alpha);
}

bool indicator(int x, int y, int **actual_matrix)
{
  if((actual_player % 2 == 0 && (actual_matrix[y][x] / 100 == 2 || actual_matrix[y][x] / 10 == 2)) || (actual_player % 2 == 1 && (actual_matrix[y][x] / 100 == 1 || actual_matrix[y][x] / 10 == 1)))
  {
    int temp_x = mapToEdge(x);
    int temp_y = mapToEdge(y);
    SDL_Rect fillRect = {temp_x, temp_y, size[0], size[1]};
    gTexture = loadTexture("Sprites/Indicator.png");
    setBlendMode(SDL_BLENDMODE_BLEND);
    setAlpha(120);
    SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
    setAlpha(150);
    for(int u = 0; u < 15; u++)
    {
      for(int v = 0; v < 15; v++)
      {
        if(check_move(y, x, u, v, actual_matrix) != impossible)
        {
          int temp_u = mapToEdge(u);
          int temp_v = mapToEdge(v);
          getSize(temp_u, temp_v);
          SDL_Rect _fillRect = {temp_v, temp_u, size[1], size[0]};
          SDL_RenderCopy(gRenderer, gTexture, NULL, &_fillRect);
        }
      }
    }
    SDL_DestroyTexture(gTexture);
    gTexture = NULL;
    SDL_RenderPresent(gRenderer);
    return true;
  }
  return false;
}

void createBoard(int **actual_matrix)
{
  init();
  int x, y, u, v, move = 0, p[3];
  bool quit = false, game_over = false;
  mainMenu(&quit);
  if(!quit)
  {
    draw(actual_matrix);
  }
  SDL_Event event;
  while(!quit)
  {
    gameOver(actual_matrix, &game_over);
    if(SDL_WaitEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        quit = true;
      }
      else if(event.type == SDL_MOUSEBUTTONDOWN && !game_over)
      {
        if(move % 2 == 0)
        {
          SDL_GetMouseState(&y, &x);
          if(x <= 580 && y <= 580)
          {
            getSize(y, x);
            y = mapToInt(y);
            x = mapToInt(x);
            if(indicator(y, x, actual_matrix))
            {
              move++;
            }
          }
        }
        else if(move % 2 == 1)
        {
          SDL_GetMouseState(&v, &u);
          if(u <= 580 && v <= 580)
          {
            getSize(v, u);
            v = mapToInt(v);
            u = mapToInt(u);
            apply_move(x, y, u, v, &actual_matrix);
            move++;
            draw(actual_matrix);
          }
        }
      }
    }
  }
  close();
}
