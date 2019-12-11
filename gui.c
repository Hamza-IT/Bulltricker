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
        gTexture = loadTexture("Sprites/King.png");
      }
      else if(actual_matrix[l][k] == 120 || actual_matrix[l][k] == 121 || actual_matrix[l][k] == 220 || actual_matrix[l][k] == 221)
      {
        gTexture = loadTexture("Sprites/Queen.png");
      }
      else if(actual_matrix[l][k] == 110 || actual_matrix[l][k] == 210)
      {
        gTexture = loadTexture("Sprites/Pawn_r.png");
      }
      else if(actual_matrix[l][k] == 111 || actual_matrix[l][k] == 211)
      {
        gTexture = loadTexture("Sprites/Pawn.png");
      }
      else if(actual_matrix[l][k] == 23)
      {
        gTexture = loadTexture("Sprites/King.png");
      }
      SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
      SDL_DestroyTexture(gTexture);
      gTexture = NULL;
    }
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
          SDL_GetMouseState(&x, &y);
          x /= 30;
          y /= 30;
          move++;
        }
        else if(move % 2 == 1)
        {
          SDL_GetMouseState(&u, &v);
          u /= 30;
          v /= 30;
          // Still needs to check the move...
          printf("\nx = %d, y = %d\nu = %d, v = %d\n", x, y, u, v);
          apply_move(y, x, v, u, &actual_matrix);
          move++;
        }
        draw(actual_matrix);
      }
    }
  }
  close();
}
