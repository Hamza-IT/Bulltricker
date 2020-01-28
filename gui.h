#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL_mixer.h>

int SCREEN_WIDTH;
int SCREEN_HEIGHT;
Mix_Chunk *sMove[2];
Mix_Chunk *sMandatory;

void draw(int **);
void createBoard(int ***);

#endif
