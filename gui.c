#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "gui.h"

int screen_width;
int screen_height;

SDL_Window *gWindow;
SDL_Surface *gScreenSurface;

typedef enum _bool
{
  false,
  true
} bool;

void init()
{
  FILE * fptr = fopen("screen_settings.txt", "r");
  fscanf(fptr, "%d %d", &screen_width, &screen_height);
  fclose(fptr);
  SDL_Init(SDL_INIT_VIDEO);
  gWindow = SDL_CreateWindow("Bulltricker", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN);
  gScreenSurface = SDL_GetWindowSurface(gWindow);
}

void end()
{
  SDL_FreeSurface(gScreenSurface);
  gScreenSurface = NULL;
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  SDL_Quit();
}

void createWindow()
{
  bool quit = false;
  SDL_Event e;
  init();
  while(!quit)
  {
    while(SDL_PollEvent(&e) != 0)
    {
      if(e.type == SDL_QUIT)
      {
        quit = true;
      }
    }
    SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface -> format, 0x00, 0x00, 0x00));
    SDL_UpdateWindowSurface(gWindow);
  }
  end();
}
