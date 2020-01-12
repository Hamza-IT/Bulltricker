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

void loadMedia()
{
  return;
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
  int x,y;
  for(int i = 0, k = 0; i < SCREEN_WIDTH, k < 15; k++)
  {
    for(int j = 0, l = 0; j < SCREEN_HEIGHT, l < 15; l++)
    {
      if(actual_matrix[l][k] == -1)
      {
        SDL_Rect fillRect = {i, j, 15, 15};
        gTexture = loadTexture("Sprites/NP.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = y = 15;
      }
      else if(actual_matrix[l][k] == 1)
      {
        SDL_Rect fillRect = {i, j, 30, 15};
        gTexture = loadTexture("Sprites/P1.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = 30;
        y = 15;
      }
      else if(actual_matrix[l][k] == 2)
      {
        SDL_Rect fillRect = {i, j, 15, 30};
        gTexture = loadTexture("Sprites/P2.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = 15;
        y = 30;
      }
      else if(actual_matrix[l][k] == -2)
      {
        SDL_Rect fillRect = {i, j, 30, 30};
        gTexture = loadTexture("Sprites/PK.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = y = 30;
      }
      else if(actual_matrix[l][k] == 13)
      {
        SDL_Rect fillRect = {i, j, 30, 30};
        gTexture = loadTexture("Sprites/KB.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = y = 30;
      }
      else if(actual_matrix[l][k] == 23)
      {
        SDL_Rect fillRect = {i, j, 30, 30};
        gTexture = loadTexture("Sprites/KW.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = y = 30;
      }
      else if(actual_matrix[l][k] == 121)
      {
        SDL_Rect fillRect = {i, j, 30, 15};
        gTexture = loadTexture("Sprites/QBR.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = 30;
        y = 15;
      }
      else if(actual_matrix[l][k] == 120)
      {
        SDL_Rect fillRect = {i, j, 15, 30};
        gTexture = loadTexture("Sprites/QB.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = 15;
        y = 30;
      }
      else if(actual_matrix[l][k] == 221)
      {
        SDL_Rect fillRect = {i, j, 30, 15};
        gTexture = loadTexture("Sprites/QWR.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = 30;
        y = 15;
      }
      else if(actual_matrix[l][k] == 220)
      {
        SDL_Rect fillRect = {i, j, 15, 30};
        gTexture = loadTexture("Sprites/QW.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = 15;
        y = 30;
      }
      else if(actual_matrix[l][k] == 110)
      {
        SDL_Rect fillRect = {i, j, 15, 30};
        gTexture = loadTexture("Sprites/PBR.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = 15;
        y = 30;
      }
      else if(actual_matrix[l][k] == 210)
      {
        SDL_Rect fillRect = {i, j, 15, 30};
        gTexture = loadTexture("Sprites/PWR.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = 15;
        y = 30;
      }
      else if(actual_matrix[l][k] == 111)
      {
        SDL_Rect fillRect = {i, j, 30, 15};
        gTexture = loadTexture("Sprites/PB.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = 30;
        y = 15;
      }
      else
      {
        SDL_Rect fillRect = {i, j, 30, 15};
        gTexture = loadTexture("Sprites/PW.png");
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x = 30;
        y = 15;
      }
      SDL_DestroyTexture(gTexture);
      gTexture = NULL;
      j += y;
    }
    i += x;
  }
  SDL_RenderPresent(gRenderer);
}

void createBoard(int **actual_matrix)
{
  init();
  loadMedia();
  draw(actual_matrix);
  int x, y, u, v, move = 0;
  bool quit = false;
  SDL_Event event;
  while(!quit)
  {
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
