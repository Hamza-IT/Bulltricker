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

typedef enum _bool
{
  false,
  true
} bool;

void init()
{
  FILE * fptr = fopen("screen_settings.txt", "r");
  fscanf(fptr, "%d %d", &SCREEN_WIDTH, &SCREEN_HEIGHT);
  fclose(fptr);
  SDL_Init(SDL_INIT_VIDEO);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  gWindow = SDL_CreateWindow("Bulltricker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_PRESENTVSYNC);
  SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
  IMG_Init(IMG_INIT_PNG);
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
  SDL_Surface *temp = IMG_Load(path);
  newTexture = SDL_CreateTextureFromSurface(gRenderer, temp);
  SDL_FreeSurface(temp);
  return newTexture;
}

void draw(int **actual_matrix)
{
  SDL_RenderClear(gRenderer);
  for(int i = 0, k = 0; i < SCREEN_WIDTH, k < 15; i += 30, k++)
  {
    for(int j = 0, l = 0; j < SCREEN_HEIGHT, l < 15; j += 30, l++)
    {
      SDL_Rect fillRect = {i, j, 30, 30};
      if(actual_matrix[l][k] == -1)
      {
        gTexture = loadTexture("Sprites/NP.png");
      }
      else if(actual_matrix[l][k] == 0)
      {
        gTexture = loadTexture("Sprites/P.png");
      }
      else if(actual_matrix[l][k] == -2)
      {
        gTexture = loadTexture("Sprites/PK.png");
      }
      else if(actual_matrix[l][k] == 13)
      {
        gTexture = loadTexture("Sprites/KB.png");
      }
      else if(actual_matrix[l][k] == 23)
      {
        gTexture = loadTexture("Sprites/KW.png");
      }
      else if(actual_matrix[l][k] == 121)
      {
        gTexture = loadTexture("Sprites/QBR.png");
      }
      else if(actual_matrix[l][k] == 120)
      {
        gTexture = loadTexture("Sprites/QB.png");
      }
      else if(actual_matrix[l][k] == 221)
      {
        gTexture = loadTexture("Sprites/QWR.png");
      }
      else if(actual_matrix[l][k] == 220)
      {
        gTexture = loadTexture("Sprites/QW.png");
      }
      else if(actual_matrix[l][k] == 110)
      {
        gTexture = loadTexture("Sprites/PBR.png");
      }
      else if(actual_matrix[l][k] == 210)
      {
        gTexture = loadTexture("Sprites/PWR.png");
      }
      else if(actual_matrix[l][k] == 111)
      {
        gTexture = loadTexture("Sprites/PB.png");
      }
      else if(actual_matrix[l][k] == 211)
      {
        gTexture = loadTexture("Sprites/PW.png");
      }
      SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
      SDL_DestroyTexture(gTexture);
      gTexture = NULL;
    }
  }
  SDL_RenderPresent(gRenderer);
}

void mainMenu(bool *quit)
{
  SDL_RenderClear(gRenderer);
  SDL_Rect fillRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  SDL_Texture *textureList[4] = {loadTexture("Sprites/Main_Menu_1.png"), loadTexture("Sprites/Main_Menu_2.png"), loadTexture("Sprites/Main_Menu_3.png"), loadTexture("Sprites/Main_Menu_4.png")};
  int i = 0;
  gTexture = textureList[i];
  SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
  SDL_RenderPresent(gRenderer);
  SDL_Event event;
  bool start = false;
  while(!start)
  {
    if(SDL_WaitEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        start = true;
        *quit = true;
      }
      else if(event.type == SDL_KEYDOWN)
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_DOWN:
          {
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
            if(i == 0)
            {
              start = true;
            }
            break;
          }
        }
      }
    }
  }
}

void gameOver(int **actual_matrix)
{
  if(check_mat(actual_matrix) == 1)
  {
    return;
  }
  else if(check_mat(actual_matrix) == 2)
  {
    return;
  }
  return;
}

void createBoard(int **actual_matrix)
{
  init();
  int x, y, u, v, move = 0;
  bool quit = false;
  mainMenu(&quit);
  if(!quit)
  {
    draw(actual_matrix);
  }
  SDL_Event event;
  while(!quit)
  {
    gameOver(actual_matrix);
    if(SDL_WaitEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        quit = true;
      }
      if(event.type == SDL_MOUSEBUTTONDOWN)
      {
        if(move % 2 == 0)
        {
          SDL_GetMouseState(&y, &x);
          x /= 30;
          y /= 30;
          move++;
        }
        else if(move % 2 == 1)
        {
          SDL_GetMouseState(&v, &u);
          u /= 30;
          v /= 30;
          printf("\nx = %d, y = %d\nu = %d, v = %d\n", x, y, u, v);
          apply_move(x, y, u, v, &actual_matrix);
          move++;
        }
        draw(actual_matrix);
      }
    }
  }
  close();
}
