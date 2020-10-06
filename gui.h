#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "move.h"

typedef enum {
  NEW_GAME,
  CONTINUE,
  SETTINGS,
  EXIT,
  MENU_HOVER_COUNT
} MenuHover;

typedef enum {
  R1280x720,
  R1366x768,
  R1600x900,
  R1920x1080,
  SOUND_ON,
  SOUND_OFF,
  INDICATOR_ON,
  INDICATOR_OFF,
  ROTATION_ON,
  ROTATION_OFF,
  RETURN,
  SETTING_OPTION_COUNT
} SettingOption;

typedef enum {
  UNDO_MOVE_BUTTON,
  SAVE_GAME_BUTTON,
  LOAD_GAME_BUTTON,
  SETTINGS_BUTTON,
  MAIN_MENU_BUTTON,
  GAME_BUTTON_COUNT
} GameButton;

enum {
  BIP,
  BIP2,
  MOVE,
  MOVE2,
  MANDATORY,
  PUP,
  GAME_OVER,
  ERROR,
  SOUND_COUNT
} GameSound;

enum {
  VOID,
  BLACK,
  WHITE,
  DARK_GREY,
  LIGHT_GREY,
  GREY,
  COLOR_COUNT
} GameColor;

typedef struct {
  TTF_Font *font;
  char *name;
} Font;

int SCREEN_WIDTH;
int SCREEN_HEIGHT;

Mix_Chunk **sounds;
void show_menu_context(MenuHover);
void play_sound(Mix_Chunk *);
void draw(int *, FloatTuple, Bool);
void main_menu(int **, Bool *, Bool *, FloatTuple);
void start_game(int **);

#endif
