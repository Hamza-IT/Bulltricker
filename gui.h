#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL_mixer.h>
#include "move.h"

int SCREEN_WIDTH;
int SCREEN_HEIGHT;
Mix_Chunk *sMove[2];
Mix_Chunk *sMandatory;
Mix_Chunk *sPup;
bool mute;

void playSound(Mix_Chunk *);
void draw(int **);
void createBoard(int ***);

#endif
