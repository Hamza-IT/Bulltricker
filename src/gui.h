#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "utilities.h"

typedef enum {
  QUICK_PLAY,
  LOCAL_GAME,
  SETTINGS,
  EXIT,
  MENU_HOVER_COUNT
} MenuHover;

typedef enum {
  HOST_GAME,
  JOIN_GAME,
  LOCAL_RETURN,
  LOCAL_HOVER_COUNT
} LocalHover;

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
void draw_board(Bool);
void main_menu();
void start_game();
void new_game();

#endif
