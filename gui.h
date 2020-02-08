#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "move.h"

int SCREEN_WIDTH;
int SCREEN_HEIGHT;
SDL_Texture *gTexture;
SDL_Renderer *gRenderer;
TTF_Font *gFont;
SDL_Color black;
Mix_Chunk *sMove[2];
Mix_Chunk *sMandatory;
Mix_Chunk *sPup;
bool mute, shown;

void getSize(int, int);
void loadText(char *, SDL_Color, int, int, int ,int);
void playSound(Mix_Chunk *);
void mandatoryIndicator();
void draw(int **);
void createBoard(int ***);

#endif
