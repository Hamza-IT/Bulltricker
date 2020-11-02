#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "init.h"
#include "gui.h"
#include "move.h"
#include "thread.h"
#include "server.h"
#include "client.h"

Bool shown, mute = false, indicator_on = true, board_rotation = true, is_host = false;

char* BASE_PATH = "./Bulltricker_Data/";
char* SPRITE_PATH = "./Bulltricker_Data/Sprites/";
char* SOUND_PATH = "./Bulltricker_Data/Sounds/";
char* FONT_PATH = "./Bulltricker_Data/Fonts/";
char* SAVE_PATH = "./Bulltricker_Data/Saves/";

typedef enum {
  CONFIRM_BUTTON,
  CANCEL_BUTTON,
  JOIN_BUTTON_COUNT
} JoinButton;

typedef enum {
  MAIN,
  LOCAL
} MenuType;

SDL_Window *g_window = NULL;
SDL_Surface *g_surface = NULL;
SDL_Texture *g_texture = NULL;
SDL_Renderer *g_renderer = NULL;
Font g_font = { NULL, NULL };
SDL_Color colors[] = { {0, 0, 0}, {50, 50, 50}, {255, 255, 255}, {127, 127, 127}, {241, 241, 241}, {230, 230, 230} };

void init() {
  START_TIME = time(NULL);
  debug_log(true, __LINE__, __FILE__, "---------- Game Started at %s ----------\n\n", get_local_time());
  char path[100];
  FILE *fptr = fopen(strcat(strcpy(path, BASE_PATH), "screen_settings.txt"), "r");
  if (fptr == NULL) {
    SCREEN_WIDTH = 1280; SCREEN_HEIGHT = 720;
    debug_log(true, __LINE__, __FILE__, "\tFailed to open file at <%s>. Resolution defaulted to %dx%d.\n", path, SCREEN_WIDTH, SCREEN_HEIGHT);
  }
  else {
    fscanf(fptr, "%d %d", &SCREEN_WIDTH, &SCREEN_HEIGHT);
    fclose(fptr);
  }
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    debug_log(true, __LINE__, __FILE__, "\tFailed to initialize SDL: %s.\n", SDL_GetError());
  if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    debug_log(true, __LINE__, __FILE__, "\tFailed to set scale hint!\n");
  g_window = SDL_CreateWindow("Bulltricker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (g_window == NULL)
    debug_log(true, __LINE__, __FILE__, "\tFailed to create SDL window: %s.\n", SDL_GetError());
  g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
  if (g_renderer == NULL)
    debug_log(true, __LINE__, __FILE__, "\tFailed to create SDL renderer: %s.\n", SDL_GetError());
  SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0x00, 0x00);
  if (!IMG_Init(IMG_INIT_PNG))
    debug_log(true, __LINE__, __FILE__, "\tFailed to initialize SDL_Image: %s.\n", IMG_GetError());
  if (TTF_Init() == -1)
    debug_log(true, __LINE__, __FILE__, "\tFailed to initialize SDL_TTF: %s.\n", TTF_GetError());
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    debug_log(true, __LINE__, __FILE__, "\tFailed to initialize SDL_Mixer: %s.\n", Mix_GetError());
}

Mix_Chunk *load_wav_sound(const char *name) {
  char path[100];
  return Mix_LoadWAV(strcat(strcpy(path, SOUND_PATH), name));
}

void load_all_sounds() {
  sounds = malloc(sizeof(Mix_Chunk*)*SOUND_COUNT);
  sounds[BIP] = load_wav_sound("menu_select.wav");
  sounds[BIP2] = load_wav_sound("menu_click.wav");
  sounds[MOVE] = load_wav_sound("move_1.wav");
  sounds[MOVE2] = load_wav_sound("move_2.wav");
  sounds[MANDATORY] = load_wav_sound("mandatory.wav");
  sounds[PUP] = load_wav_sound("pawn_powerup.wav");
  sounds[GAME_OVER] = load_wav_sound("game_over.wav");
  sounds[ERROR] = load_wav_sound("error.wav");
}

void play_sound(Mix_Chunk *sound) {
  if (!mute)
    Mix_PlayChannel(-1, sound, 0);
}

void close() {
  for (int i = 0; i < SOUND_COUNT; i++) {
    Mix_FreeChunk(sounds[i]);
    sounds[i] = NULL;
  }
  free(sounds);
  TTF_CloseFont(g_font.font);
  g_font = (Font) { NULL, NULL };
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
  SDL_FreeSurface(g_surface);
  g_surface = NULL;
  SDL_DestroyRenderer(g_renderer);
  g_renderer = NULL;
  SDL_DestroyWindow(g_window);
  g_window = NULL;
  destroy_mutexes();
  Mix_Quit();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  debug_log(true, __LINE__, __FILE__, "---------- Game Closed at %s ----------\n", get_local_time());
}

void set_blend_mode(SDL_BlendMode blend_mode) { SDL_SetTextureBlendMode(g_texture, blend_mode); }

void set_alpha(unsigned char alpha) { SDL_SetTextureAlphaMod(g_texture, alpha); }

void set_render_color (SDL_Color color) { SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, color.a); }

FloatTuple get_cell_size(int location) {
  float small_step = SCREEN_HEIGHT/29.f; // bigger_cell_height = 3 * smaller_cell_height and 8 smaller cells + 7 bigger cells in one column.
  float big_step = small_step*3.f;
  if ((location/column_count) % 2 == 0) {
    if (location % 2 == 0)
      return (FloatTuple) { small_step, small_step };
    else
      return (FloatTuple) { big_step, small_step };
  }
  else {
    if (location % 2 == 1)
      return (FloatTuple) { small_step, big_step };
    else
      return (FloatTuple) { big_step, big_step };
  }
}

int mirror(int index) {
  SDL_LockMutex(join_mutex);
  if (local_ready == true)
    return local_player == WHITE_PLAYER ? index : mirror_position(index);
  SDL_UnlockMutex(join_mutex);
  if (current_player == BLACK_PLAYER && board_rotation == true)
    return mirror_position(index);
  return index;
}

int map_to_int(IntTuple position) {
  float step = SCREEN_HEIGHT/29.f;
  int max_count = row_count;
  if (max_count < column_count) { max_count = column_count; }
  float edges[max_count];
  edges[0] = step;
  float x, y;
  Bool x_found = false, y_found = false;
  for (int i = 1; i < max_count; i++) {
    int mult = i % 2 == 0 ? 1 : 3;
    edges[i] = edges[i-1]+(step*mult);
  }
  for (int i = 0; i < max_count; i++) {
    if (x_found == false && position.x < edges[i]) {
      x = i;
      x_found = true;
    }
    if (y_found == false && position.y < edges[i]) {
      y = i;
      y_found = true;
    }
    if (x_found == true && y_found == true) { break; }
  }
  return mirror(y*column_count+x);
}

IntTuple map_to_edge(int location) {
  float step = SCREEN_HEIGHT/29.f;
  int max_count = row_count;
  if (max_count < column_count) { max_count = column_count; }
  float edges[max_count];
  edges[0] = 0;
  edges[1] = step;
  int y = location / column_count, x = location % column_count;
  for (int i = 2; i < max_count; i++) {
    int mult = i % 2 == 0 ? 3 : 1;
    edges[i] = edges[i-1]+(step*mult);
  }
  return (IntTuple) { edges[x]+offset.x, edges[y]+offset.y };
}

void load_font(char *name, unsigned char size) {
  char path[100];
  TTF_Font *new_font = TTF_OpenFont(strcat(strcpy(path, FONT_PATH), name), size);
  if (new_font == NULL) {
    debug_log(true, __LINE__, __FILE__, "\tFailed to load font %s: %s.\n", name, TTF_GetError());
    return;
  }
  if (g_font.font != NULL)
    TTF_CloseFont(g_font.font);
  g_font.font = new_font;
  g_font.name = name;
}

void change_font_size(unsigned char size) {
  if (g_font.name != NULL)
    load_font(g_font.name, size);
}

SDL_Texture *load_texture(const char *name) {
  char path[100];
  SDL_Texture *new_texture = NULL;
  new_texture = IMG_LoadTexture(g_renderer, strcat(strcpy(path, SPRITE_PATH), name));
  if (new_texture == NULL)
    debug_log(true, __LINE__, __FILE__, "\tFailed to create texture from image %s: %s.\n", name, IMG_GetError());
  return new_texture;
}

void load_text(const char *text, SDL_Color color, int x, int y, unsigned char a) {
  int width, height;
  TTF_SizeText(g_font.font, text, &width, &height);
  SDL_Rect text_rect = { x, y, width, height };
  g_surface = TTF_RenderText_Blended(g_font.font, text, color);
  g_texture = SDL_CreateTextureFromSurface(g_renderer, g_surface);
  set_alpha(a);
  SDL_FreeSurface(g_surface);
  g_surface = NULL;
  SDL_RenderCopy(g_renderer, g_texture, NULL, &text_rect);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
}

void player_indication(float grid_x, float grid_width, Bool rotated) {
  FloatTuple white_height = rotated == false ? (FloatTuple) { SCREEN_HEIGHT*0.875f, SCREEN_HEIGHT*0.930f } : (FloatTuple) { SCREEN_HEIGHT*0.03f, SCREEN_HEIGHT*0.085f };
  FloatTuple black_height = rotated == false ? (FloatTuple) { SCREEN_HEIGHT*0.03f, SCREEN_HEIGHT*0.085f } : (FloatTuple) { SCREEN_HEIGHT*0.875f, SCREEN_HEIGHT*0.930f };
  load_text("Player 1", colors[DARK_GREY], grid_x+grid_width*0.4f, white_height.x, 255);
  load_text("Whites", colors[DARK_GREY], grid_x+grid_width*0.575f, white_height.y, 255);
  load_text("Player 2", colors[DARK_GREY], grid_x+grid_width*0.4f, black_height.x, 255);
  load_text("Blacks", colors[DARK_GREY], grid_x+grid_width*0.575f, black_height.y, 255);
}

void load_grid() {
  float grid_x = SCREEN_HEIGHT+2*offset.x;
  float grid_width = SCREEN_WIDTH-(SCREEN_HEIGHT+2*offset.x);
  SDL_Rect hide_rect = { grid_x, 0, grid_width, SCREEN_HEIGHT };
  g_texture = load_texture("Hide_Text.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &hide_rect);
  SDL_DestroyTexture(g_texture);
  const int grid_count = 7;
  SDL_FRect f_dark_rects[] = { (SDL_FRect) { grid_x, 0, grid_width, SCREEN_HEIGHT/13.f*2 }, (SDL_FRect) { grid_x, SCREEN_HEIGHT/13.f*2, grid_width, SCREEN_HEIGHT/13.f*2 }, (SDL_FRect) { grid_x, SCREEN_HEIGHT/13.f*4, grid_width, SCREEN_HEIGHT/13.f }, (SDL_FRect) { grid_x, SCREEN_HEIGHT/13.f*5, grid_width, SCREEN_HEIGHT/13.f*3 }, (SDL_FRect) { grid_x, SCREEN_HEIGHT/13.f*8, grid_width, SCREEN_HEIGHT/13.f }, (SDL_FRect) { grid_x, SCREEN_HEIGHT/13.f*9, grid_width, SCREEN_HEIGHT/13.f*2 }, (SDL_FRect) { grid_x, SCREEN_HEIGHT/13.f*11, grid_width, SCREEN_HEIGHT/13.f*2 }  };
  SDL_FRect f_light_rects[] = { (SDL_FRect) { SCREEN_HEIGHT*1.005f+2*offset.x, SCREEN_HEIGHT*0.005f, grid_width-0.01f*SCREEN_HEIGHT, SCREEN_HEIGHT/13.f*2 - SCREEN_HEIGHT*0.005f }, (SDL_FRect) { SCREEN_HEIGHT*1.005f+2*offset.x, SCREEN_HEIGHT*0.005f+SCREEN_HEIGHT/13.f*2, grid_width-0.01f*SCREEN_HEIGHT, SCREEN_HEIGHT/13.f*2-SCREEN_HEIGHT*0.005f }, (SDL_FRect) { SCREEN_HEIGHT*1.005f+2*offset.x, SCREEN_HEIGHT*0.005f+SCREEN_HEIGHT/13.f*4, grid_width-0.01f*SCREEN_HEIGHT, SCREEN_HEIGHT/13.f-SCREEN_HEIGHT*0.005f }, (SDL_FRect) { SCREEN_HEIGHT*1.005f+2*offset.x, SCREEN_HEIGHT*0.005f+SCREEN_HEIGHT/13.f*5, grid_width-0.01f*SCREEN_HEIGHT, SCREEN_HEIGHT/13.f*3-SCREEN_HEIGHT*0.005f }, (SDL_FRect) { SCREEN_HEIGHT*1.005f+2*offset.x, SCREEN_HEIGHT*0.005f+SCREEN_HEIGHT/13.f*8, grid_width-0.01f*SCREEN_HEIGHT, SCREEN_HEIGHT/13.f-SCREEN_HEIGHT*0.005f }, (SDL_FRect) { SCREEN_HEIGHT*1.005f+2*offset.x, SCREEN_HEIGHT*0.005f+SCREEN_HEIGHT/13.f*9, grid_width-0.01f*SCREEN_HEIGHT, SCREEN_HEIGHT/13.f*2-SCREEN_HEIGHT*0.005f }, (SDL_FRect) { SCREEN_HEIGHT*1.005f+2*offset.x, SCREEN_HEIGHT*0.005f+SCREEN_HEIGHT/13.f*11, grid_width-0.01f*SCREEN_HEIGHT, SCREEN_HEIGHT/13.f*2-SCREEN_HEIGHT*0.01f } };
  SDL_Color light_colors[] = { colors[GREY], colors[WHITE], colors[WHITE], colors[WHITE], colors[WHITE], colors[WHITE], colors[GREY] };
  for (int i = 0; i < grid_count; i++) {
    set_render_color(colors[DARK_GREY]);
    SDL_RenderFillRectF(g_renderer, &f_dark_rects[i]);
    set_render_color(light_colors[i]);
    SDL_RenderFillRectF(g_renderer, &f_light_rects[i]);
  }
  SDL_Texture *buttons[] = { load_texture("Undo_Button.png"), load_texture("Save_Button.png"), load_texture("Load_Button.png"), load_texture("Settings_Button.png"), load_texture("Menu_Button.png") };
  SDL_Rect b_rects[] = { { grid_x+grid_width*0.1f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f }, { grid_x+grid_width*0.275f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f }, { grid_x+grid_width*0.45f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f }, { grid_x+grid_width*0.625f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f }, { grid_x+grid_width*0.8f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f } };
  int button_start = 0;
  SDL_LockMutex(join_mutex);
  if (local_ready == true) {
    button_start = SETTINGS_BUTTON;
    b_rects[SETTINGS_BUTTON] = (SDL_Rect) { grid_x+grid_width*0.4f-SCREEN_HEIGHT/29.f*0.75f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f };
    b_rects[MAIN_MENU_BUTTON] = (SDL_Rect) { grid_x+grid_width*0.6f-SCREEN_HEIGHT/29.f*0.75f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f };
  }
  SDL_UnlockMutex(join_mutex);
  for (int i = button_start; i < GAME_BUTTON_COUNT; i++) {
    g_texture = buttons[i];
    SDL_RenderCopy(g_renderer, g_texture, NULL, &(b_rects[i]));
    SDL_DestroyTexture(buttons[i]);
    buttons[i] = NULL;
  }
  char *text = current_player == WHITE_PLAYER ? "Whites turn" : "Blacks turn";
  change_font_size(SCREEN_HEIGHT/25);
  load_text(text, colors[BLACK], grid_x+grid_width*0.35f, SCREEN_HEIGHT*0.325f, 255);
  g_texture = load_texture("Compass.png");
  SDL_Rect rect = { grid_x+grid_width*0.2f, SCREEN_HEIGHT*0.32f, SCREEN_HEIGHT/17, SCREEN_HEIGHT/17 };
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  Bool rotated = false;
  SDL_LockMutex(join_mutex);
  if (local_ready == true) {
    if (local_player == BLACK_PLAYER)
      rotated = true;
  }
  else if (board_rotation == true && current_player == BLACK_PLAYER)
    rotated = true;
  SDL_UnlockMutex(join_mutex);
  player_indication(grid_x, grid_width, rotated);
  g_texture = load_texture("Game_Pad.png");
  rect = (SDL_Rect) { grid_x+grid_width*0.15f, SCREEN_HEIGHT*0.03f, SCREEN_HEIGHT/10, SCREEN_HEIGHT/10 };
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  rect.y = SCREEN_HEIGHT*0.875f;
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
  set_render_color(colors[VOID]);
}

void fill_blank() {
  SDL_Rect rect = { 0, 0, offset.x, SCREEN_HEIGHT };
  g_texture = load_texture("Board_Edge.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  rect.x = SCREEN_HEIGHT+offset.x;
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
}

void draw_board(Bool clear_render) {
  if (clear_render == true)
    SDL_RenderClear(g_renderer);
  set_render_color(colors[VOID]);
  load_grid();
  fill_blank();
  float x = offset.x, y = offset.y;
  for (int i = 0; i < get_board_size(); i++) {
    int target = mirror(i);
    FloatTuple cell_size = get_cell_size(target);
    Bool is_rotated = (target / column_count) % 2 == 1 ? true : false;
    if (current_board[target] == BLOCKED)
      g_texture = load_texture("NP.png");
    else if (current_board[target] == EMPTY_KING)
      g_texture = load_texture("PK.png");
    else if (current_board[target] == WHITE_KING)
      g_texture = load_texture("KW.png");
    else if (current_board[target] == BLACK_KING)
      g_texture = load_texture("KB.png");
    else if (is_rotated == true) {
      if (current_board[target] == EMPTY_NORMAL)
        g_texture = load_texture("P0.png");
      else if (current_board[target] == BLACK_QUEEN)
        g_texture = load_texture("QB.png");
      else if (current_board[target] == WHITE_QUEEN)
        g_texture = load_texture("QW.png");
      else if (current_board[target] == BLACK_PAWN)
        g_texture = load_texture("PBR.png");
      else if (current_board[target] == WHITE_PAWN)
        g_texture = load_texture("PWR.png");
    }
    else {
      if (current_board[target] == EMPTY_NORMAL)
        g_texture = load_texture("P1.png");
      else if (current_board[target] == BLACK_QUEEN)
        g_texture = load_texture("QBR.png");
      else if (current_board[target] == WHITE_QUEEN)
        g_texture = load_texture("QWR.png");
      else if (current_board[target] == BLACK_PAWN)
        g_texture = load_texture("PB.png");
      else if (current_board[target] == WHITE_PAWN)
        g_texture = load_texture("PW.png");
    }
    SDL_Rect cell_rect = { x, y, cell_size.x, cell_size.y };
    SDL_RenderCopy(g_renderer, g_texture, NULL, &cell_rect);
    SDL_DestroyTexture(g_texture);
    g_texture = NULL;
    x += cell_size.x;
    if (((int)x)+1 >= SCREEN_HEIGHT+offset.x) {
      y += cell_size.y;
      x = offset.x;
    }
  }
  SDL_RenderPresent(g_renderer);
}

Bool save_game(GameState current_state) {
  if (current_state == ONGOING) {
    char path[100];
    FILE *fptr = fopen(strcat(strcpy(path, SAVE_PATH), "Game.save"), "wb");
    if (fptr != NULL) {
      for (int i = 0; i < get_board_size(); i++)
        fwrite(&(current_board[i]), sizeof(int), 1, fptr);
      fwrite(&current_player, sizeof(Player), 1, fptr);
      fclose(fptr);
      return true;
    }
    else
      debug_log(true, __LINE__, __FILE__, "\tFailed to open file at <%s>. Game could not be saved.\n", path);
  }
  return false;
}

Bool load_game() {
  if (current_board != NULL) {
    char path[100];
    FILE *fptr = fopen(strcat(strcpy(path, SAVE_PATH), "Game.save"), "rb");
    if (fptr != NULL) {
      for (int i = 0; i < get_board_size(); i++)
        fread(&(current_board[i]), sizeof(int), 1, fptr);
      fread(&current_player, sizeof(Player), 1, fptr);
      fclose(fptr);
      draw_board(true);
      get_allowed_actions();
      return true;
    }
    else
      debug_log(true, __LINE__, __FILE__, "\n\tFailed to load file at <%s>. Game could not be loaded.\n", path);
  }
  return false;
}

void game_over_screen(GameState current_state, int grid_x, int grid_width) {
  if (current_state != ONGOING) {
    char *text;
    int text_width;
    TTF_SizeText(g_font.font, "Game Over", &text_width, NULL);
    load_text("Game Over", colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.41, 255);
    if (current_state == WHITE_WIN)
      text = "Player 1 Wins";
    else if (current_state == BLACK_WIN)
      text = "Player 2 Wins";
    else if (current_state == DRAW)
      text = "Draw";
    TTF_SizeText(g_font.font, text, &text_width, NULL);
    play_sound(sounds[GAME_OVER]);
    load_text(text, colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.475, 255);
    SDL_RenderPresent(g_renderer);
  }
}

IntTuple map_to_resolution(SettingOption option) {
  IntTuple resolution;
  switch (option) {
    case R1280x720:
      resolution = (IntTuple) { 1280, 720 };
      break;
    case R1366x768:
      resolution = (IntTuple) { 1366, 768 };
      break;
    case R1600x900:
      resolution = (IntTuple) { 1600, 900 };
      break;
    case R1920x1080:
      resolution = (IntTuple) { 1920, 1080 };
      break;
  }
  return resolution;
}

SettingOption map_resolution() {
  SettingOption option;
  if (SCREEN_WIDTH == 1280 && SCREEN_HEIGHT == 720) { option = R1280x720; }
  else if (SCREEN_WIDTH == 1366 && SCREEN_HEIGHT == 768) { option = R1366x768; }
  else if (SCREEN_WIDTH == 1600 && SCREEN_HEIGHT == 900) { option = R1600x900; }
  else if (SCREEN_WIDTH == 1920 && SCREEN_HEIGHT == 1080) { option = R1920x1080; }
  return option;
}

void show_settings_context(SDL_Rect **button_rects) {
  SDL_RenderPresent(g_renderer);
  SDL_RenderClear(g_renderer);
  g_texture = load_texture("Black_BG.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
  int text_width;
  change_font_size(SCREEN_HEIGHT/15);
  TTF_SizeText(g_font.font, "Settings", &text_width, NULL);
  load_text("Settings", colors[WHITE], (SCREEN_WIDTH-text_width)/2, SCREEN_HEIGHT*0.045f, 255);
  change_font_size(SCREEN_HEIGHT/25);
  TTF_SizeText(g_font.font, "Screen Resolution", &text_width, NULL);
  load_text("Screen Resolution", colors[WHITE], (SCREEN_WIDTH*0.6f-text_width)/2, SCREEN_HEIGHT*0.25f, 255);
  SDL_Rect rect = { (SCREEN_WIDTH*0.5f-text_width)/2, SCREEN_HEIGHT*0.25f, SCREEN_HEIGHT*0.05f, SCREEN_HEIGHT*0.05f };
  g_texture = load_texture("Resolution_Setting.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  int origin_x = (SCREEN_WIDTH*1.1f-text_width)/2, origin_y = SCREEN_HEIGHT*0.21f, origin_w = SCREEN_HEIGHT*0.04f, origin_h = SCREEN_HEIGHT*0.04f;
  char *texts[] = { "1280x720", "1366x768", "1600x900", "1920x1080", "On", "Off", "On", "Off", "On", "Off" };
  FloatTuple text_coords[] = { (FloatTuple) { origin_x*1.075f, origin_y*0.985f }, (FloatTuple) { origin_x*1.075f, SCREEN_HEIGHT*0.31f*0.985f }, (FloatTuple) { (SCREEN_WIDTH*1.55f-text_width)/2*1.05f, origin_y*0.985f }, (FloatTuple) { (SCREEN_WIDTH*1.55f-text_width)/2*1.05f, SCREEN_HEIGHT*0.31f*0.985f }, (FloatTuple) { origin_x*1.075f, SCREEN_HEIGHT*0.435f*0.99f  }, (FloatTuple) { (SCREEN_WIDTH*1.55f-text_width)/2*1.05f, SCREEN_HEIGHT*0.435f*0.99f }, (FloatTuple) { origin_x*1.075f, SCREEN_HEIGHT*0.57f*0.99f }, (FloatTuple) { (SCREEN_WIDTH*1.55f-text_width)/2*1.05f, SCREEN_HEIGHT*0.57f*0.99f }, (FloatTuple) { origin_x*1.075f, SCREEN_HEIGHT*0.705f*0.99f }, (FloatTuple) { (SCREEN_WIDTH*1.55f-text_width)/2*1.05f, SCREEN_HEIGHT*0.705f*0.99f } };
  (*button_rects)[R1280x720] = (SDL_Rect) { origin_x, origin_y, origin_w, origin_h }; (*button_rects)[R1366x768] = (SDL_Rect) { origin_x, SCREEN_HEIGHT*0.31f, origin_w, origin_h }; (*button_rects)[R1600x900] = (SDL_Rect) { (SCREEN_WIDTH*1.55f-text_width)/2, origin_y, origin_w, origin_h }; (*button_rects)[R1920x1080] = (SDL_Rect) { (SCREEN_WIDTH*1.55f-text_width)/2, SCREEN_HEIGHT*0.31f, origin_w, origin_h }; (*button_rects)[SOUND_ON] = (SDL_Rect) { origin_x, SCREEN_HEIGHT*0.435f, origin_w, origin_h }; (*button_rects)[SOUND_OFF] = (SDL_Rect) { (SCREEN_WIDTH*1.55f-text_width)/2, SCREEN_HEIGHT*0.435f, origin_w, origin_h  }; (*button_rects)[INDICATOR_ON] = (SDL_Rect) { origin_x, SCREEN_HEIGHT*0.57f, origin_w, origin_h }; (*button_rects)[INDICATOR_OFF] = (SDL_Rect) { (SCREEN_WIDTH*1.55f-text_width)/2, SCREEN_HEIGHT*0.57f, origin_w, origin_h }, (*button_rects)[ROTATION_ON] = (SDL_Rect) { origin_x, SCREEN_HEIGHT*0.705f, origin_w, origin_h }; (*button_rects)[ROTATION_OFF] = (SDL_Rect) { (SCREEN_WIDTH*1.55f-text_width)/2, SCREEN_HEIGHT*0.705f, origin_w, origin_h };
  SDL_Texture *off_button = load_texture("Radio_Button_Off.png"), *on_button = load_texture("Radio_Button_On.png");
  for (int i = 0; i < SETTING_OPTION_COUNT-1; i++) {
    load_text(texts[i], colors[WHITE], text_coords[i].x, text_coords[i].y, 255);
    if (i == map_resolution() || (i == SOUND_ON && mute == false) || (i == SOUND_OFF && mute == true) || (i == INDICATOR_ON && indicator_on == true) || (i == INDICATOR_OFF && indicator_on == false) || (i == ROTATION_ON && board_rotation == true) || (i == ROTATION_OFF && board_rotation == false))
      SDL_RenderCopy(g_renderer, on_button, NULL, (*button_rects)+i);
    else
      SDL_RenderCopy(g_renderer, off_button, NULL, (*button_rects)+i);
  }
  TTF_SizeText(g_font.font, "Sound", &text_width, NULL);
  load_text("Sound", colors[WHITE], (SCREEN_WIDTH*0.6f-text_width)/2, SCREEN_HEIGHT*0.429f, 255);
  rect.x = (SCREEN_WIDTH*0.5f-text_width)/2; rect.y = SCREEN_HEIGHT*0.429f;
  g_texture = load_texture("Sound_Setting.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  TTF_SizeText(g_font.font, "Indicator", &text_width, NULL);
  load_text("Indicator", colors[WHITE], (SCREEN_WIDTH*0.6f-text_width)/2, SCREEN_HEIGHT*0.562f, 255);
  rect.x = (SCREEN_WIDTH*0.5f-text_width)/2; rect.y = SCREEN_HEIGHT*0.562f;
  g_texture = load_texture("Indicator_Setting.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  TTF_SizeText(g_font.font, "Board Rotation", &text_width, NULL);
  load_text("Board Rotation", colors[WHITE], (SCREEN_WIDTH*0.6f-text_width)/2, SCREEN_HEIGHT*0.697f, 255);
  rect.x = (SCREEN_WIDTH*0.5f-text_width)/2; rect.y = SCREEN_HEIGHT*0.697f;
  g_texture = load_texture("Rotation_Setting.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  SDL_DestroyTexture(off_button); SDL_DestroyTexture(on_button);
  TTF_SizeText(g_font.font, "Return", &text_width, NULL);
  g_texture = load_texture("Return_Button.png");
  (*button_rects)[RETURN] = (SDL_Rect) { SCREEN_WIDTH*0.855f*0.5f, SCREEN_HEIGHT*0.835f, SCREEN_WIDTH*0.145f, SCREEN_HEIGHT*0.1f };
  SDL_RenderCopy(g_renderer, g_texture, NULL, (*button_rects)+RETURN);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
  load_text("Return", colors[BLACK], (SCREEN_WIDTH-text_width)/2, SCREEN_HEIGHT*0.86f, 255);
  SDL_RenderPresent(g_renderer);
}

void settings_return(Bool *quit_settings, Bool in_game) {
  SDL_Delay(100);
  play_sound(sounds[BIP2]);
  *quit_settings = true;
  if (in_game == true)
    draw_board(true);
  else
    show_menu_context(SETTINGS);
}

void settings_screen(Bool in_game) {
  SDL_Delay(100);
  Bool quit_settings = false;
  SDL_Rect *button_rects = malloc(sizeof(SDL_Rect)*SETTING_OPTION_COUNT);
  SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW), *hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
  show_settings_context(&button_rects);
  SDL_Event event;
  SettingOption current_hover = -1;
  while (quit_settings == false && quit == false) {
    if (SDL_WaitEvent(&event)) {
      if (event.type == SDL_QUIT)
        quit = true;
      else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        settings_return(&quit_settings, in_game);
      else if (event.type == SDL_MOUSEMOTION) {
        int x, y;
        Bool is_hovering = false;
        SDL_GetMouseState(&x, &y);
        for (int i = 0; i < SETTING_OPTION_COUNT; i++) {
          if (x >= button_rects[i].x && x <= button_rects[i].x+button_rects[i].w && y >= button_rects[i].y && y <= button_rects[i].y+button_rects[i].h) {
            current_hover = i;
            SDL_SetCursor(hand_cursor);
            is_hovering = true;
          }
        }
        if (is_hovering == false) {
          current_hover = -1;
          SDL_SetCursor(arrow_cursor);
        }
      }
      else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (current_hover >= R1280x720 && current_hover <= R1920x1080) {
          IntTuple res = map_to_resolution(current_hover);
          SCREEN_WIDTH = res.x; SCREEN_HEIGHT = res.y;
          char path[100];
          FILE *fptr = fopen(strcat(strcpy(path, BASE_PATH), "screen_settings.txt"), "w");
          if (fptr == NULL) { debug_log(true, __LINE__, __FILE__, "\tFailed to open file at <%s>. Screen settings were not saved.\n", path); }
          else {
            fprintf(fptr, "%d %d\nSCREEN_WIDTH SCREEN_HEIGHT\n", SCREEN_WIDTH, SCREEN_HEIGHT);
            fclose(fptr);
            debug_log(true, __LINE__, __FILE__, "\tScreen resolution changed to %dx%d. Screen Settings were saved correctly.\n\n", SCREEN_WIDTH, SCREEN_HEIGHT);
          }
          SDL_SetWindowSize(g_window, SCREEN_WIDTH, SCREEN_HEIGHT);
          SDL_SetWindowPosition(g_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
        else if (current_hover == SOUND_ON) { mute = false; }
        else if (current_hover == SOUND_OFF) { mute = true; }
        else if (current_hover == INDICATOR_ON) { indicator_on = true; }
        else if (current_hover == INDICATOR_OFF) { indicator_on = false; }
        else if (current_hover == ROTATION_ON) { board_rotation = true; }
        else if (current_hover == ROTATION_OFF) { board_rotation = false; }
        else if (current_hover == RETURN) { settings_return(&quit_settings, in_game); }
        if (current_hover != -1) {
          if (current_hover != SOUND_OFF && current_hover != RETURN) { play_sound(sounds[BIP2]); }
          if (current_hover != RETURN) { show_settings_context(&button_rects); }
        }
      }
    }
  }
  SDL_FreeCursor(arrow_cursor); SDL_FreeCursor(hand_cursor);
}

void show_menu_context(MenuHover current) {
  SDL_RenderClear(g_renderer);
  change_font_size(SCREEN_HEIGHT/20);
  SDL_Rect fill_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
  g_texture = load_texture("Main_Menu.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &fill_rect);
  SDL_DestroyTexture(g_texture);
  int width;
  char *texts[] = { "Quick Play", "Local Game", "Settings", "Exit" };
  int y_pos[] = { SCREEN_HEIGHT*0.63, SCREEN_HEIGHT*0.72, SCREEN_HEIGHT*0.81, SCREEN_HEIGHT*0.9 };
  for (int i = 0; i < MENU_HOVER_COUNT; i++) {
    float alpha_mult = i == current ? 1 : 0.8;
    TTF_SizeText(g_font.font, texts[i], &width, NULL);
    load_text(texts[i], colors[WHITE], (SCREEN_WIDTH-width)/2, y_pos[i], 255*alpha_mult);
  }
  SDL_RenderPresent(g_renderer);
}

void new_game() {
  on_main_menu = false;
  set_render_color(colors[VOID]);
  SDL_Delay(150);
  debug_log(true, __LINE__, __FILE__, "\t---------- Started a New Round ----------\n\n");
  SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
  SDL_SetCursor(arrow_cursor);
  play_sound(sounds[BIP2]);
  current_player = WHITE_PLAYER;
  game_over = false;
  mandatory_pawn_move = false;
  mandatory_queen_move = false;
  shown = false;
  free(current_board);
  current_board = initialize_board();
  start = true;
  draw_board(true);
  get_allowed_actions();
  SDL_FreeCursor(arrow_cursor);
}

void continue_game() {
  SDL_Delay(150);
  if (load_game() == true) {
    SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    SDL_SetCursor(arrow_cursor);
    play_sound(sounds[BIP2]);
    start = true;
    shown = false;
    SDL_FreeCursor(arrow_cursor);
  }
  else
    play_sound(sounds[ERROR]);
}

void show_local_context(LocalHover current) {
  SDL_DestroyTexture(g_texture);
  SDL_RenderClear(g_renderer);
  change_font_size(SCREEN_HEIGHT/20);
  SDL_Rect fill_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
  g_texture = load_texture("Main_Menu.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &fill_rect);
  SDL_DestroyTexture(g_texture);
  int width;
  char *texts[] = { "Host Game", "Join Game", "Return" };
  int y_pos[] = { SCREEN_HEIGHT*0.65, SCREEN_HEIGHT*0.75, SCREEN_HEIGHT*0.85 };
  for (int i = 0; i < LOCAL_HOVER_COUNT; i++) {
    float alpha_mult = i == current ? 1 : 0.8;
    TTF_SizeText(g_font.font, texts[i], &width, NULL);
    load_text(texts[i], colors[WHITE], (SCREEN_WIDTH-width)/2, y_pos[i], 255*alpha_mult);
  }
  SDL_RenderPresent(g_renderer);
}

void open_settings() {
  SDL_Delay(150);
  SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
  SDL_SetCursor(arrow_cursor);
  play_sound(sounds[BIP2]);
  settings_screen(false);
  if (quit == false)
    main_menu(MAIN);
  SDL_FreeCursor(arrow_cursor);
}

void exit_game() {
  play_sound(sounds[BIP2]);
  SDL_Delay(100);
  quit = true;
}

void show_host_context(SDL_Rect *button_rect, const char *ip_info) {
  SDL_RenderPresent(g_renderer);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
  SDL_RenderClear(g_renderer);
  change_font_size(SCREEN_HEIGHT/15);
  SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
  SDL_SetCursor(arrow_cursor);
  SDL_FreeCursor(arrow_cursor);
  SDL_Rect fill_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
  g_texture = load_texture("Black_BG.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &fill_rect);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
  int width;
  TTF_SizeText(g_font.font, "Hosted Game", &width, NULL);
  load_text("Hosted Game", colors[WHITE], (SCREEN_WIDTH-width)*0.5f, SCREEN_HEIGHT*0.05f, 255);
  TTF_SizeText(g_font.font, "Waiting for an opponent ...", &width, NULL);
  load_text("Waiting for an opponent ...", colors[WHITE], (SCREEN_WIDTH-width)/2, SCREEN_HEIGHT*0.3f, 255);
  TTF_SizeText(g_font.font, ip_info, &width, NULL);
  load_text(ip_info, colors[WHITE], (SCREEN_WIDTH-width)/2, SCREEN_HEIGHT*0.5f, 255);
  TTF_SizeText(g_font.font, "Cancel", &width, NULL);
  g_texture = load_texture("Return_Button.png");
  (*button_rect) = (SDL_Rect) { SCREEN_WIDTH*0.8f*0.5f, SCREEN_HEIGHT*0.8f, SCREEN_WIDTH*0.2f, SCREEN_HEIGHT*0.11f };
  SDL_RenderCopy(g_renderer, g_texture, NULL, button_rect);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
  load_text("Cancel", colors[BLACK], (SCREEN_WIDTH-width)/2, SCREEN_HEIGHT*0.8125f, 255);
  SDL_RenderPresent(g_renderer);
}

void host_return() {
  SDL_LockMutex(quit_mutex);
  quit_local = true;
  close_socket();
  SDL_UnlockMutex(quit_mutex);
  SDL_Delay(100);
  play_sound(sounds[BIP2]);
  main_menu(LOCAL);
}

void host_screen(const char *ip) {
  Bool return_hover = false;
  SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW), *hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
  SDL_Event event;
  SDL_Rect return_rect;
  char ip_info[50];
  Bool quit_host = false;
  strcpy(ip_info, "Host's IP address: "); strcat(ip_info, ip);
  show_host_context(&return_rect, ip_info);
  SDL_LockMutex(join_mutex);
  while (quit == false && quit_local == false && local_ready == false && quit_host == false) {
    if (SDL_WaitEventTimeout(&event, 1)) {
      if (event.type == SDL_QUIT)
        quit = true;
      else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        quit_host = true;
      else if (event.type == SDL_MOUSEMOTION) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        if (x >= return_rect.x && x <= return_rect.x+return_rect.w && y >= return_rect.y && y <= return_rect.y+return_rect.h) {
          return_hover = true;
          SDL_SetCursor(hand_cursor);
        }
        else {
          return_hover = false;
          SDL_SetCursor(arrow_cursor);
        }
      }
      else if (event.type == SDL_MOUSEBUTTONDOWN && return_hover == true)
        quit_host = true;
    }
    SDL_CondWaitTimeout(join_cond, join_mutex, 1);
    SDL_Delay(1);
  }
  if (local_ready == true) {
    local_player = WHITE_PLAYER;
    SDL_UnlockMutex(join_mutex);
    new_game();
  }
  else
    SDL_UnlockMutex(join_mutex);
  SDL_FreeCursor(arrow_cursor); SDL_FreeCursor(hand_cursor);
  if (quit_host == true)
    host_return();
}

void update_input(const char *input_text, const SDL_Rect *rect, SDL_Color color) {
  g_texture = load_texture("Input_Bar.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, rect);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
  change_font_size(SCREEN_HEIGHT/22);
  int text_width;
  TTF_SizeText(g_font.font, input_text, &text_width, NULL);
  if (*input_text != '\0')
    load_text(input_text, color, (SCREEN_WIDTH-text_width)*0.5f, (*rect).y+SCREEN_HEIGHT*0.01f, 255);
  else
    load_text(" ", color, (SCREEN_WIDTH-text_width)*0.5f, (*rect).y+SCREEN_HEIGHT*0.01f, 255);
  SDL_RenderPresent(g_renderer);
}

char *client_screen(char *default_text) {
  show_local_context(-1);
  SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW), *hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND), *text_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
  SDL_SetCursor(arrow_cursor);
  set_render_color(colors[LIGHT_GREY]);
  SDL_Rect rect = { SCREEN_WIDTH*0.3f, SCREEN_HEIGHT*0.35f, SCREEN_WIDTH*0.4f, SCREEN_HEIGHT*0.35f };
  SDL_RenderFillRect(g_renderer, &rect);
  set_render_color(colors[WHITE]);
  rect = (SDL_Rect) { SCREEN_WIDTH*0.35f, SCREEN_HEIGHT*0.475f, SCREEN_WIDTH*0.3f, SCREEN_HEIGHT*0.075f };
  SDL_RenderFillRect(g_renderer, &rect);
  SDL_Rect button_rects[] = { (SDL_Rect) { SCREEN_WIDTH*0.36f, SCREEN_HEIGHT*0.58f, SCREEN_WIDTH*0.13f, SCREEN_HEIGHT*0.09f }, (SDL_Rect) { SCREEN_WIDTH*0.51f, SCREEN_HEIGHT*0.58f, SCREEN_WIDTH*0.13f, SCREEN_HEIGHT*0.09f }};
  for (int i = 0; i < JOIN_BUTTON_COUNT; i++)
    SDL_RenderFillRect(g_renderer, button_rects+i);
  int text_width;
  TTF_SizeText(g_font.font, "Host's IP Address", &text_width, NULL);
  load_text("Host's IP Address", colors[BLACK], (SCREEN_WIDTH-text_width)*0.5f, SCREEN_HEIGHT*0.38f, 255);
  change_font_size(SCREEN_HEIGHT/22);
  TTF_SizeText(g_font.font, "Confirm", &text_width, NULL);
  load_text("Confirm", colors[DARK_GREY], SCREEN_WIDTH*0.425f-text_width*0.5f, SCREEN_HEIGHT*0.595f, 255);
  TTF_SizeText(g_font.font, "Cancel", &text_width, NULL);
  load_text("Cancel", colors[DARK_GREY], SCREEN_WIDTH*0.575f-text_width*0.5f, SCREEN_HEIGHT*0.595f, 255);
  SDL_RenderPresent(g_renderer);
  Bool quit_join = false, confirmed = false;
  SDL_Event event;
  JoinButton current_hover = -1;
  char *input_text = malloc(16);
  if (default_text != NULL)
    update_input(default_text, &rect, colors[DARK_GREY]);
  memset(input_text, 0x00, 16);
  SDL_StartTextInput();
  while (quit == false && quit_join == false) {
    Bool is_hovering = false;
    if (SDL_WaitEvent(&event)) {
      if (event.type == SDL_QUIT)
        quit = true;
      else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          quit_join = true;
          play_sound(sounds[BIP2]);
          show_local_context(JOIN_GAME);
        }
        else if (event.key.keysym.sym == SDLK_RETURN) {
          if (strlen(input_text) >= 7) {
            confirmed = true;
            quit_join = true;
          }
          else
            play_sound(sounds[ERROR]);
        }
        else if (event.key.keysym.sym == SDLK_BACKSPACE) {
          if (strlen(input_text) > 0)
            input_text = pop_back(input_text);
          update_input(input_text, &rect, colors[BLACK]);
        }
        else if (event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
          SDL_SetClipboardText(input_text);
        else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
          char *clipboard = SDL_GetClipboardText();
          size_t length = strlen(clipboard);
          if (length > 0 && length <= 15) {
            input_text = clipboard;
            update_input(input_text, &rect, colors[BLACK]);
          }
        }
      }
      else if (event.type == SDL_TEXTINPUT) {
        if (!(SDL_GetModState() & KMOD_CTRL && (event.text.text[0] == 'c' || event.text.text[0] == 'C' || event.text.text[0] == 'v' || event.text.text[0] == 'V')) && strlen(input_text) < 15) {
          strcat(input_text, event.text.text);
          update_input(input_text, &rect, colors[BLACK]);
        }
      }
      else if (event.type == SDL_MOUSEMOTION) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        for (int i = 0; i < JOIN_BUTTON_COUNT; i++) {
          if (x >= button_rects[i].x && x <= button_rects[i].x+button_rects[i].w && y >= button_rects[i].y && y <= button_rects[i].y+button_rects[i].h) {
            current_hover = i;
            is_hovering = true;
            SDL_SetCursor(hand_cursor);
          }
        }
        if (is_hovering == false) {
          current_hover = -1;
          SDL_SetCursor(arrow_cursor);
        }
      }
      else if (event.type == SDL_MOUSEBUTTONDOWN) {
        switch (current_hover) {
          case CONFIRM_BUTTON:
            if (strlen(input_text) >= 7) {
              confirmed = true;
              quit_join = true;
            }
            else
              play_sound(sounds[ERROR]);
            break;
          case CANCEL_BUTTON:
            quit_join = true;
            SDL_SetCursor(arrow_cursor);
            show_local_context(JOIN_GAME);
            play_sound(sounds[BIP2]);
            break;
        }
      }
    }
    SDL_Delay(1);
  }
  SDL_StopTextInput();
  SDL_FreeCursor(arrow_cursor); SDL_FreeCursor(hand_cursor); SDL_FreeCursor(text_cursor);
  SDL_RenderClear(g_renderer);
  return confirmed == true ? input_text : NULL;
}

void host_game() {
  play_sound(sounds[BIP2]);
  SDL_LockMutex(quit_mutex);
  quit_local = false;
  SDL_UnlockMutex(quit_mutex);
  is_host = true;
  SDL_LockMutex(ip_mutex);
  SDL_CreateThread(server_routine, "Server Routine", NULL); // Server Thread
  while (ip_updated == false)
    SDL_CondWait(ip_cond, ip_mutex);
  SDL_UnlockMutex(ip_mutex);
  ClientData client_data = { "127.0.0.1" };
  ClientData *cdptr = &client_data;
  SDL_CreateThread(client_routine, "Hosting Client Routine", (void*)cdptr); // Host Client Thread
  char *server_ip = get_win_ip();
  if (server_ip == NULL)
    server_ip = "0.0.0.0";
  host_screen(server_ip);
}

void join_game(char *text) {
  SDL_LockMutex(quit_mutex);
  quit_local = false;
  SDL_UnlockMutex(quit_mutex);
  char *ip = client_screen(text);
  if (ip != NULL) {
    ClientData data = { ip };
    ClientData *dptr = &data;
    SDL_LockMutex(join_mutex);
    SDL_CreateThread(client_routine, "Non-Hosting Client Routine", (void*)dptr); // Non-Host Client Thread
    while (local_ready == false && local_failed == false)
      SDL_CondWait(join_cond, join_mutex);
    if (local_failed == true) {
      local_failed = false;
      SDL_UnlockMutex(join_mutex);
      play_sound(sounds[ERROR]);
      join_game("Failed to join");
    }
    else if (local_ready == true) {
      local_player = BLACK_PLAYER;
      SDL_UnlockMutex(join_mutex);
      new_game();
    }
    else
      SDL_UnlockMutex(join_mutex);
  }
}

void main_menu(MenuType type) {
  SDL_LockMutex(quit_mutex);
  quit = quit_local = game_over = start = is_host = false;
  SDL_UnlockMutex(quit_mutex);
  SDL_LockMutex(join_mutex);
  local_ready = ip_updated = move_applied = local_failed = false;
  SDL_UnlockMutex(join_mutex);
  on_main_menu = true;
  MenuHover current_hover = -1;
  if (type == MAIN)
    show_menu_context(current_hover);
  else if (type == LOCAL)
    show_local_context(current_hover);
  SDL_Event event;
  SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW), *hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
  Bool at_local = (Bool)type;
  destroy_mutexes();
  init_mutexes();
  while (start == false && quit == false && on_main_menu == true) {
    if (SDL_WaitEventTimeout(&event, 1)) {
      if (event.type == SDL_QUIT)
        quit = true;
      else if (event.type == SDL_MOUSEMOTION) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        Bool is_hovering = false;
        int text_width, text_height;
        if (at_local == false) {
          char *texts[] = { "Quick Play", "Local Game", "Settings", "Exit" };
          int y_origins[] = { SCREEN_HEIGHT*0.63, SCREEN_HEIGHT*0.72, SCREEN_HEIGHT*0.81, SCREEN_HEIGHT*0.9 };
          for (int i = 0; i < MENU_HOVER_COUNT; i++) {
            TTF_SizeText(g_font.font, texts[i], &text_width, &text_height);
            if (y >= y_origins[i] && y <= y_origins[i]+text_height && x >= (SCREEN_WIDTH-text_width)/2 && x <= (SCREEN_WIDTH+text_width)/2) {
              if (current_hover != i) {
                SDL_Delay(50);
                SDL_SetCursor(hand_cursor);
                play_sound(sounds[BIP]);
                current_hover = i;
                show_menu_context(current_hover);
              }
              is_hovering = true;
            }
          }
        }
        else {
          char *texts[] = { "Host Game", "Join Game", "Return" };
          int y_origins[] = { SCREEN_HEIGHT*0.65, SCREEN_HEIGHT*0.75, SCREEN_HEIGHT*0.85 };
          for (int i = 0; i < LOCAL_HOVER_COUNT; i++) {
            TTF_SizeText(g_font.font, texts[i], &text_width, &text_height);
            if (y >= y_origins[i] && y <= y_origins[i]+text_height && x >= (SCREEN_WIDTH-text_width)/2 && x <= (SCREEN_WIDTH+text_width)/2) {
              if (current_hover != i) {
                SDL_Delay(50);
                SDL_SetCursor(hand_cursor);
                play_sound(sounds[BIP]);
                current_hover = i;
                show_local_context(current_hover);
              }
              is_hovering = true;
            }
          }
        }
        if (current_hover != -1 && is_hovering == false) {
          current_hover = -1;
          if (at_local == false)
            show_menu_context(-1);
          else
            show_local_context(-1);
          SDL_SetCursor(arrow_cursor);
        }
      }
      else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (at_local == false) {
          if (current_hover == QUICK_PLAY)
            new_game();
          else if (current_hover == LOCAL_GAME) {
            current_hover = -1;
            show_local_context(current_hover);
            SDL_SetCursor(arrow_cursor);
            at_local = true;
            play_sound(sounds[BIP2]);
          }
          else if (current_hover == SETTINGS) {
            open_settings();
          }
          else if (current_hover == EXIT)
            exit_game();
        }
        else {
          if (current_hover == HOST_GAME) {
            on_main_menu = false;
            break;
          }
          else if (current_hover == JOIN_GAME) {
            play_sound(sounds[BIP2]);
            join_game(NULL);
          }
          else if (current_hover == LOCAL_RETURN) {
            at_local = false;
            current_hover = -1;
            SDL_SetCursor(arrow_cursor);
            show_menu_context(current_hover);
            play_sound(sounds[BIP2]);
          }
        }
      }
      else if (event.type == SDL_KEYDOWN) {
        int current_hover_count = at_local == false ? MENU_HOVER_COUNT : LOCAL_HOVER_COUNT;
        switch (event.key.keysym.sym) {
          case SDLK_DOWN:
            SDL_Delay(50);
            play_sound(sounds[BIP]);
            current_hover = (current_hover + 1) % current_hover_count;
            if (at_local == false)
              show_menu_context(current_hover);
            else
              show_local_context(current_hover);
            break;
          case SDLK_UP:
            SDL_Delay(50);
            play_sound(sounds[BIP]);
            current_hover = ((current_hover - 1) + current_hover_count) % current_hover_count;
            if (at_local == false)
              show_menu_context(current_hover);
            else
              show_local_context(current_hover);
            break;
          case SDLK_ESCAPE:
            if (at_local == true) {
              play_sound(sounds[BIP2]);
              at_local = false;
              current_hover = -1;
              SDL_SetCursor(arrow_cursor);
              show_menu_context(current_hover);
            }
            break;
          case SDLK_RETURN:
            if (at_local == false) {
              if (current_hover == QUICK_PLAY)
                new_game();
              else if (current_hover == LOCAL_GAME) {
                current_hover = -1;
                show_local_context(current_hover);
                SDL_SetCursor(arrow_cursor);
                at_local = true;
                play_sound(sounds[BIP2]);
              }
              else if (current_hover == SETTINGS)
                open_settings();
              else if (current_hover == EXIT)
                exit_game();
            }
            else {
              if (current_hover == HOST_GAME) {
                on_main_menu = false;
                break;
              }
              else if (current_hover == JOIN_GAME) {
                play_sound(sounds[BIP2]);
                join_game(NULL);
              }
              else if (current_hover == LOCAL_RETURN) {
                at_local = false;
                current_hover = -1;
                show_menu_context(current_hover);
                play_sound(sounds[BIP2]);
              }
            }
            break;
        }
      }
    }
    SDL_Delay(1);
  }
  if (on_main_menu == false && start == false)
    host_game();
  SDL_FreeCursor(arrow_cursor); SDL_FreeCursor(hand_cursor);
}

Bool action_indicator(int index) {
  SDL_LockMutex(join_mutex);
  if (local_ready == true && current_player != local_player) {
    SDL_UnlockMutex(join_mutex);
    return false;
  }
  SDL_UnlockMutex(join_mutex);
  get_allowed_piece_actions(index);
  if (current_board[index]/COLOR_DIVIDER == current_player) {
    if (indicator_on == true) {
      index = mirror(index);
      IntTuple edge_map = map_to_edge(index);
      FloatTuple cell_size = get_cell_size(index);
      SDL_Rect rect = { edge_map.x, edge_map.y, cell_size.x, cell_size.y };
      g_texture = load_texture("_Indicator.png");
      set_blend_mode(SDL_BLENDMODE_MOD);
      set_alpha(120);
      SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
      SDL_DestroyTexture(g_texture);
      g_texture = NULL;
      g_texture = load_texture("Indicator.png");
      set_blend_mode(SDL_BLENDMODE_BLEND);
      set_alpha(150);
      for (int i = 0; i < allowed_piece_count; i++) {
        int target = mirror(current_allowed_piece[i]);
        edge_map = map_to_edge(target);
        cell_size = get_cell_size(target);
        rect = (SDL_Rect) { edge_map.x, edge_map.y, cell_size.x, cell_size.y };
        SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
      }
      SDL_DestroyTexture(g_texture);
      g_texture = NULL;
      SDL_RenderPresent(g_renderer);
    }
    return true;
  }
  return false;
}

void mandatory_indicator(int cell_index, int grid_x, int grid_width) {
  if ((mandatory_pawn_move == true || mandatory_queen_move == true) && shown == false) {
    SDL_Rect rects[allowed_count];
    for (int i = 0; i < allowed_count; i++) {
      if (cell_index == current_allowed[i].x) { return; }
      int target = mirror(current_allowed[i].x);
      IntTuple edge_map = map_to_edge(target);
      FloatTuple cell_size = get_cell_size(target);
      rects[i] = (SDL_Rect) { edge_map.x, edge_map.y, cell_size.x, cell_size.y };
    }
    int text_width;
    TTF_SizeText(g_font.font, "Mandatory Move", &text_width, NULL);
    load_text("Mandatory Move", colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.41, 255);
    g_texture = load_texture("_Mandatory.png");
    set_blend_mode(SDL_BLENDMODE_MOD);
    set_alpha(150);
    for (int i = 0; i < allowed_count; i++) { SDL_RenderCopy(g_renderer, g_texture, NULL, rects+i); }
    SDL_DestroyTexture(g_texture);
    g_texture = NULL;
    play_sound(sounds[MANDATORY]);
    SDL_RenderPresent(g_renderer);
  }
}

void hide_text(int grid_x, int grid_width) {
  SDL_Rect rect = { grid_x+grid_width*0.1, SCREEN_HEIGHT*0.55, grid_width*0.8, SCREEN_HEIGHT*0.05 };
  g_texture = load_texture("Hide_Text.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
}

void try_send_quit() {
  SDL_LockMutex(join_mutex);
  if (local_ready == true) {
    SDL_UnlockMutex(join_mutex);
    uint8_t data[] = { 0 };
    client_send_data(data, sizeof(uint8_t), QUIT_FLAG);
  }
  else
    SDL_UnlockMutex(join_mutex);
}

void start_game() {
  init();
  load_all_sounds();
  load_font("bodoni_mtblack.ttf", SCREEN_HEIGHT/20);
  init_mutexes();
  int x, y, text_width, last_active_cell = -1;
  offset = (FloatTuple) { SCREEN_HEIGHT*0.1f, 0 };
  Bool log_shown = false;
  SDL_Event event;
  main_menu(MAIN);
  GameButton current_hover = -1;
  SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW), *hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
  char *texts[] = { "Undo Move", "Save Game", "Load Game", "Settings", "Main Menu" };
  SDL_LockMutex(quit_mutex);
  SDL_LockMutex(move_mutex);
  while (quit == false) {
    float grid_x = SCREEN_HEIGHT+2*offset.x;
    float grid_width = SCREEN_WIDTH-(SCREEN_HEIGHT+2*offset.x);
    SDL_Rect b_rects[] = { { grid_x+grid_width*0.1f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f }, { grid_x+grid_width*0.275f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f }, { grid_x+grid_width*0.45f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f }, { grid_x+grid_width*0.625f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f }, { grid_x+grid_width*0.8f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f } };
    int button_start = 0;
    SDL_LockMutex(join_mutex);
    if (local_ready == true) {
      button_start = SETTINGS_BUTTON;
      b_rects[SETTINGS_BUTTON] = (SDL_Rect) { grid_x+grid_width*0.4f-SCREEN_HEIGHT/29.f*0.75f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f };
      b_rects[MAIN_MENU_BUTTON] = (SDL_Rect) { grid_x+grid_width*0.6f-SCREEN_HEIGHT/29.f*0.75f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f };
    }
    SDL_UnlockMutex(join_mutex);
    Bool is_hovering = false;
    if (SDL_WaitEventTimeout(&event, 1)) {
      if (event.type == SDL_QUIT)
        quit = true;
      else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          SDL_Delay(100);
          play_sound(sounds[BIP2]);
          SDL_RenderClear(g_renderer);
          if (SDL_GetCursor() == hand_cursor)
            SDL_SetCursor(arrow_cursor);
          try_send_quit();
          SDL_UnlockMutex(quit_mutex);
          main_menu(MAIN);
          SDL_LockMutex(quit_mutex);
        }
      }
      else if (event.type == SDL_MOUSEMOTION) {
        SDL_GetMouseState(&x, &y);
        for (int i = button_start; i < GAME_BUTTON_COUNT; i++) {
          if (x >= b_rects[i].x && x <= b_rects[i].x+b_rects[i].w && y >= b_rects[i].y && y <= b_rects[i].y+b_rects[i].h && log_shown == false) {
            SDL_SetCursor(hand_cursor);
            TTF_SizeText(g_font.font, texts[i], &text_width, NULL);
            load_text(texts[i], colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.55, 255);
            SDL_RenderPresent(g_renderer);
            current_hover = i;
            is_hovering = true;
          }
        }
        if (is_hovering == false) {
          hide_text(grid_x, grid_width);
          SDL_SetCursor(arrow_cursor);
          current_hover = -1;
          log_shown = false;
          SDL_RenderPresent(g_renderer);
        }
      }
      else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (current_hover == UNDO_MOVE_BUTTON)
          undo_last_move();
        else if (current_hover == SAVE_GAME_BUTTON) {
          Bool has_saved = save_game(check_game_state());
          if (has_saved == true) {
            play_sound(sounds[BIP2]);
            hide_text(grid_x, grid_width);
            SDL_RenderPresent(g_renderer);
            TTF_SizeText(g_font.font, "Game Saved", &text_width, NULL);
            load_text("Game Saved", colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.55, 255);
            SDL_RenderPresent(g_renderer);
            log_shown = true;
          }
          else {
            play_sound(sounds[ERROR]);
            hide_text(grid_x, grid_width);
            SDL_RenderPresent(g_renderer);
            TTF_SizeText(g_font.font, "Failed To Save", &text_width, NULL);
            load_text("Failed To Save", colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.55, 255);
            SDL_RenderPresent(g_renderer);
            log_shown = true;
          }
        }
        else if (current_hover == LOAD_GAME_BUTTON) {
          Bool has_loaded = load_game();
          if (has_loaded == true) {
            play_sound(sounds[BIP2]);
            hide_text(grid_x, grid_width);
            SDL_RenderPresent(g_renderer);
            TTF_SizeText(g_font.font, "Game Loaded", &text_width, NULL);
            load_text("Game Loaded", colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.55, 255);
            SDL_RenderPresent(g_renderer);
            log_shown = true;
          }
          else {
            play_sound(sounds[ERROR]);
            hide_text(grid_x, grid_width);
            SDL_RenderPresent(g_renderer);
            TTF_SizeText(g_font.font, "Failed To Load", &text_width, NULL);
            load_text("Failed To Load", colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.55, 255);
            SDL_RenderPresent(g_renderer);
            log_shown = true;
          }
        }
        else if (current_hover == SETTINGS_BUTTON) {
          play_sound(sounds[BIP2]);
          settings_screen(true);
        }
        else if (current_hover == MAIN_MENU_BUTTON) {
          play_sound(sounds[BIP2]);
          SDL_SetCursor(arrow_cursor);
          SDL_RenderClear(g_renderer);
          try_send_quit();
          SDL_UnlockMutex(quit_mutex);
          main_menu(MAIN);
          SDL_LockMutex(quit_mutex);
        }
        else if (current_hover == -1 && game_over == false) {
          if (x >= offset.x && x <= SCREEN_HEIGHT+offset.x && y >= offset.y && y <= SCREEN_HEIGHT+offset.y) {
            int cell_index = map_to_int((IntTuple) { x-offset.x, y-offset.y });
            draw_board(false); // Clears Highlighted Path
            if (last_active_cell == cell_index) {
              play_sound(sounds[MOVE2]);
              last_active_cell = -1;
            }
            else {
              Bool found = false;
              if (last_active_cell != -1) {
                for (int i = 0; i < allowed_piece_count; i++) {
                  if (current_allowed_piece[i] == cell_index) {
                    found = true;
                    GameState state = apply_move((IntTuple) { last_active_cell, cell_index });
                    game_over = state != ONGOING ? true : false;
                    game_over_screen(state, grid_x, grid_width);
                    if (local_ready == true) {
                      uint8_t data[] = { last_active_cell, cell_index };
                      client_send_data(data, 2*sizeof(uint8_t), MOVE_FLAG);
                    }
                    break;
                  }
                }
              }
              if (found == false && last_active_cell != cell_index && action_indicator(cell_index) == true) {
                mandatory_indicator(cell_index, grid_x, grid_width);
                last_active_cell = cell_index;
                play_sound(sounds[MOVE]);
              }
              else
                last_active_cell = -1;
            }
          }
        }
      }
    }
    SDL_LockMutex(join_mutex);
    if (local_ready == true) {
      SDL_UnlockMutex(join_mutex);
      SDL_CondWaitTimeout(move_cond, move_mutex, 1);
      if (move_applied == true) {
        GameState state = apply_move((IntTuple) { client.origin_cell, client.target_cell });
        game_over = state != ONGOING ? true : false;
        game_over_screen(state, grid_x, grid_width);
        move_applied = false;
      }
      SDL_CondWaitTimeout(quit_cond, quit_mutex, 1);
      if (quit_local == true) {
        SDL_UnlockMutex(quit_mutex);
        main_menu(LOCAL);
        SDL_LockMutex(quit_mutex);
      }
    }
    else
      SDL_UnlockMutex(join_mutex);
    SDL_Delay(1);
  }
  SDL_UnlockMutex(move_mutex);
  try_send_quit();
  SDL_UnlockMutex(quit_mutex);
  if (is_host == false)
    close_socket();
  else {
    SDL_LockMutex(quit_mutex);
    while (quit_ready == false)
      SDL_CondWait(quit_cond, quit_mutex);
    quit_ready = false;
    SDL_UnlockMutex(quit_mutex);
  }
  SDL_FreeCursor(arrow_cursor); SDL_FreeCursor(hand_cursor);
  close();
}
