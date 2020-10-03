#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gui.h"
#include "init.h"
#include "utilities.h"

Bool shown, mute = FALSE, indicator_on = TRUE;

char* BASE_PATH = "./Bulltricker_Data/";
char* SPRITE_PATH = "./Bulltricker_Data/Sprites/";
char* SOUND_PATH = "./Bulltricker_Data/Sounds/";
char* FONT_PATH = "./Bulltricker_Data/Fonts/";
char* SAVE_PATH = "./Bulltricker_Data/Saves/";

SDL_Window *g_window = NULL;
SDL_Surface *g_surface = NULL;
SDL_Texture *g_texture = NULL;
SDL_Renderer *g_renderer = NULL;
Font g_font = { NULL, NULL };
SDL_Color colors[] = { {0, 0, 0}, {50, 50, 50}, {255, 255, 255}, {127, 127, 127}, {241, 241, 241}, {230, 230, 230} };

void init() {
  START_TIME = time(NULL);
  struct tm * time_info;
  time_info = localtime(&START_TIME);
  log_text("---------- Game Started at %s ----------\n\n", trim(asctime(time_info)));
  char path[100];
  FILE *fptr = fopen(strcat(strcpy(path, BASE_PATH), "screen_settings.txt"), "r");
  if (fptr == NULL) {
    SCREEN_WIDTH = 1280; SCREEN_HEIGHT = 720;
    log_text("\tFailed to open file at <%s>. Resolution defaulted to %dx%d.\n", path, SCREEN_WIDTH, SCREEN_HEIGHT);
  }
  else {
    fscanf(fptr, "%d %d", &SCREEN_WIDTH, &SCREEN_HEIGHT);
    fclose(fptr);
  }
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    log_text("\tFailed to initialize SDL: %s.\n", SDL_GetError());
  if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    log_text("\tFailed to set scale hint!\n");
  g_window = SDL_CreateWindow("Bulltricker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (g_window == NULL)
    log_text("\tFailed to create SDL window: %s.\n", SDL_GetError());
  g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
  if (g_renderer == NULL)
    log_text("\tFailed to create SDL renderer: %s.\n", SDL_GetError());
  SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0x00, 0x00);
  if (!IMG_Init(IMG_INIT_PNG))
    log_text("\tFailed to initialize SDL_Image: %s.\n", IMG_GetError());
  if (TTF_Init() == -1)
    log_text("\tFailed to initialize SDL_TTF: %s.\n", TTF_GetError());
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    log_text("\tFailed to initialize SDL_Mixer: %s.\n", Mix_GetError());
}

Mix_Chunk *load_wav_sound(char *name) {
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
  Mix_Quit();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  log_text("\n---------- Game Closed Successfully ----------\n");
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

int map_to_int(IntTuple position) {
  float step = SCREEN_HEIGHT/29.f;
  int max_count = row_count;
  if (max_count < column_count) { max_count = column_count; }
  float edges[max_count];
  edges[0] = step;
  float x, y;
  Bool x_found = FALSE, y_found = FALSE;
  for (int i = 1; i < max_count; i++) {
    int mult = i % 2 == 0 ? 1 : 3;
    edges[i] = edges[i-1]+(step*mult);
  }
  for (int i = 0; i < max_count; i++) {
    if (x_found == FALSE && position.x < edges[i]) {
      x = i;
      x_found = TRUE;
    }
    if (y_found == FALSE && position.y < edges[i]) {
      y = i;
      y_found = TRUE;
    }
    if (x_found == TRUE && y_found == TRUE) { break; }
  }
  return y*column_count+x;
}

IntTuple map_to_edge(int location, FloatTuple offset) {
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
    log_text("\tFailed to load font %s: %s.\n", name, TTF_GetError());
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

SDL_Texture *load_texture(char *name) {
  char path[100];
  SDL_Texture *new_texture = NULL;
  new_texture = IMG_LoadTexture(g_renderer, strcat(strcpy(path, SPRITE_PATH), name));
  if (new_texture == NULL)
    log_text("\tFailed to create texture from image %s: %s.\n", name, IMG_GetError());
  return new_texture;
}

void load_text(char *text, SDL_Color color, int x, int y, unsigned char a) {
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

void load_grid(FloatTuple offset) {
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
  for (int i = 0; i < GAME_BUTTON_COUNT; i++) {
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
  load_text("Player 1", colors[DARK_GREY], grid_x+grid_width*0.4f, SCREEN_HEIGHT*0.875f, 255);
  load_text("Whites", colors[DARK_GREY], grid_x+grid_width*0.575f, SCREEN_HEIGHT*0.930f, 255);
  load_text("Player 2", colors[DARK_GREY], grid_x+grid_width*0.4f, SCREEN_HEIGHT*0.03f, 255);
  load_text("Blacks", colors[DARK_GREY], grid_x+grid_width*0.575f, SCREEN_HEIGHT*0.085f, 255);
  g_texture = load_texture("Game_Pad.png");
  rect = (SDL_Rect) { grid_x+grid_width*0.15f, SCREEN_HEIGHT*0.03f, SCREEN_HEIGHT/10, SCREEN_HEIGHT/10 };
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  rect.y = SCREEN_HEIGHT*0.875f;
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
  set_render_color(colors[VOID]);
}

void fill_blank(FloatTuple offset) {
  SDL_Rect rect = { 0, 0, offset.x, SCREEN_HEIGHT };
  g_texture = load_texture("Board_Edge.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  rect.x = SCREEN_HEIGHT+offset.x;
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
}

void draw(int *current_board, FloatTuple offset, Bool clear_render) {
  if (clear_render == TRUE)
    SDL_RenderClear(g_renderer);
  load_grid(offset);
  fill_blank(offset);
  float x = offset.x, y = offset.y;
  for (int i = 0; i < get_board_size(); i++) {
    FloatTuple cell_size = get_cell_size(i);
    Bool is_rotated = (i / column_count) % 2 == 1 ? TRUE : FALSE;
    if (current_board[i] == BLOCKED)
      g_texture = load_texture("NP.png");
    else if (current_board[i] == EMPTY_KING)
      g_texture = load_texture("PK.png");
    else if (current_board[i] == WHITE_KING)
      g_texture = load_texture("KW.png");
    else if (current_board[i] == BLACK_KING)
      g_texture = load_texture("KB.png");
    else if (is_rotated == TRUE) {
      if (current_board[i] == EMPTY_NORMAL)
        g_texture = load_texture("P0.png");
      else if (current_board[i] == BLACK_QUEEN)
        g_texture = load_texture("QB.png");
      else if (current_board[i] == WHITE_QUEEN)
        g_texture = load_texture("QW.png");
      else if (current_board[i] == BLACK_PAWN)
        g_texture = load_texture("PBR.png");
      else if (current_board[i] == WHITE_PAWN)
        g_texture = load_texture("PWR.png");
    }
    else {
      if (current_board[i] == EMPTY_NORMAL)
        g_texture = load_texture("P1.png");
      else if (current_board[i] == BLACK_QUEEN)
        g_texture = load_texture("QBR.png");
      else if (current_board[i] == WHITE_QUEEN)
        g_texture = load_texture("QWR.png");
      else if (current_board[i] == BLACK_PAWN)
        g_texture = load_texture("PB.png");
      else if (current_board[i] == WHITE_PAWN)
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

Bool save_game(int *current_board, GameState current_state) {
  if (current_state == ONGOING) {
    char path[100];
    FILE *fptr = fopen(strcat(strcpy(path, SAVE_PATH), "Game.save"), "wb");
    if (fptr != NULL) {
      for (int i = 0; i < get_board_size(); i++)
        fwrite(&(current_board[i]), sizeof(int), 1, fptr);
      fwrite(&current_player, sizeof(Player), 1, fptr);
      fclose(fptr);
      return TRUE;
    }
    else
      log_text("\tFailed to open file at <%s>. Game could not be saved.\n");
  }
  return FALSE;
}

Bool load_game(int **current_board, FloatTuple(offset)) {
  if ((*current_board) != NULL) {
    char path[100];
    FILE *fptr = fopen(strcat(strcpy(path, SAVE_PATH), "Game.save"), "rb");
    if (fptr != NULL) {
      for (int i = 0; i < get_board_size(); i++)
        fread(&((*current_board)[i]), sizeof(int), 1, fptr);
      fread(&current_player, sizeof(Player), 1, fptr);
      fclose(fptr);
      draw(*current_board, offset, TRUE);
      get_allowed_actions(*current_board);
      return TRUE;
    }
    else
      log_text("\n\tFailed to load file at <%s>. Game could not be loaded.\n", path);
  }
  return FALSE;
}

void game_over_screen(int *current_board, GameState current_state, int grid_x, int grid_width) {
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
  if (option == R1280x720) { return (IntTuple) { 1280, 720 }; }
  if (option == R1366x768) { return (IntTuple) { 1366, 768 }; }
  if (option == R1600x900) { return (IntTuple) { 1600, 900 }; }
  if (option == R1920x1080) { return (IntTuple) { 1920, 1080 }; }
}

SettingOption map_resolution() {
  if (SCREEN_WIDTH == 1280 && SCREEN_HEIGHT == 720) { return R1280x720; }
  else if (SCREEN_WIDTH == 1366 && SCREEN_HEIGHT == 768) { return R1366x768; }
  else if (SCREEN_WIDTH == 1600 && SCREEN_HEIGHT == 900) { return R1600x900; }
  else if (SCREEN_WIDTH == 1920 && SCREEN_HEIGHT == 1080) { return R1920x1080; }
}

void show_settings_context(SDL_Rect **button_rects) {
  SDL_RenderPresent(g_renderer);
  SDL_RenderClear(g_renderer);
  g_texture = load_texture("Menu_Settings.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
  int text_width;
  change_font_size(SCREEN_HEIGHT/15);
  TTF_SizeText(g_font.font, "Settings", &text_width, NULL);
  load_text("Settings", colors[WHITE], (SCREEN_WIDTH-text_width)/2, SCREEN_HEIGHT*0.05f, 255);
  change_font_size(SCREEN_HEIGHT/25);
  TTF_SizeText(g_font.font, "Screen Resolution", &text_width, NULL);
  load_text("Screen Resolution", colors[WHITE], (SCREEN_WIDTH*0.6f-text_width)/2, SCREEN_HEIGHT*0.3f, 255);
  SDL_Rect rect = { (SCREEN_WIDTH*0.5f-text_width)/2, SCREEN_HEIGHT*0.3f, SCREEN_HEIGHT*0.05f, SCREEN_HEIGHT*0.05f };
  g_texture = load_texture("Resolution_Setting.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  int origin_x = (SCREEN_WIDTH*1.1f-text_width)/2, origin_y = SCREEN_HEIGHT*0.26f, origin_w = SCREEN_HEIGHT*0.04f, origin_h = SCREEN_HEIGHT*0.04f;
  char *texts[] = { "1280x720", "1366x768", "1600x900", "1920x1080", "On", "Off", "On", "Off" };
  FloatTuple text_coords[] = { (FloatTuple) { origin_x*1.075f, origin_y*0.985f }, (FloatTuple) { origin_x*1.075f, SCREEN_HEIGHT*0.36f*0.985f }, (FloatTuple) { (SCREEN_WIDTH*1.55f-text_width)/2*1.05f, origin_y*0.985f }, (FloatTuple) { (SCREEN_WIDTH*1.55f-text_width)/2*1.05f, SCREEN_HEIGHT*0.36f*0.985f }, (FloatTuple) { origin_x*1.075f, SCREEN_HEIGHT*0.505f*0.99f  }, (FloatTuple) { (SCREEN_WIDTH*1.55f-text_width)/2*1.05f, SCREEN_HEIGHT*0.505f*0.99f }, (FloatTuple) { origin_x*1.075f, SCREEN_HEIGHT*0.655f*0.99f }, (FloatTuple) { (SCREEN_WIDTH*1.55f-text_width)/2*1.05f, SCREEN_HEIGHT*0.655f*0.99f } };
  (*button_rects)[R1280x720] = (SDL_Rect) { origin_x, origin_y, origin_w, origin_h }; (*button_rects)[R1366x768] = (SDL_Rect) { origin_x, SCREEN_HEIGHT*0.36f, origin_w, origin_h }; (*button_rects)[R1600x900] = (SDL_Rect) { (SCREEN_WIDTH*1.55f-text_width)/2, origin_y, origin_w, origin_h }; (*button_rects)[R1920x1080] = (SDL_Rect) { (SCREEN_WIDTH*1.55f-text_width)/2, SCREEN_HEIGHT*0.36f, origin_w, origin_h }; (*button_rects)[SOUND_ON] = (SDL_Rect) { origin_x, SCREEN_HEIGHT*0.505f, origin_w, origin_h }; (*button_rects)[SOUND_OFF] = (SDL_Rect) { (SCREEN_WIDTH*1.55f-text_width)/2, SCREEN_HEIGHT*0.505f, origin_w, origin_h  }; (*button_rects)[INDICATOR_ON] = (SDL_Rect) { origin_x, SCREEN_HEIGHT*0.655f, origin_w, origin_h }; (*button_rects)[INDICATOR_OFF] = (SDL_Rect) { (SCREEN_WIDTH*1.55f-text_width)/2, SCREEN_HEIGHT*0.655f, origin_w, origin_h };
  SDL_Texture *off_button = load_texture("Radio_Button_Off.png"), *on_button = load_texture("Radio_Button_On.png");
  for (int i = 0; i < SETTING_OPTION_COUNT-1; i++) {
    load_text(texts[i], colors[WHITE], text_coords[i].x, text_coords[i].y, 255);
    if (i == map_resolution() || (i == SOUND_ON && mute == FALSE) || (i == SOUND_OFF && mute == TRUE) || (i == INDICATOR_ON && indicator_on == TRUE) || (i == INDICATOR_OFF && indicator_on == FALSE))
      SDL_RenderCopy(g_renderer, on_button, NULL, (*button_rects)+i);
    else
      SDL_RenderCopy(g_renderer, off_button, NULL, (*button_rects)+i);
  }
  TTF_SizeText(g_font.font, "Sound", &text_width, NULL);
  load_text("Sound", colors[WHITE], (SCREEN_WIDTH*0.6f-text_width)/2, SCREEN_HEIGHT*0.5f, 255);
  rect.x = (SCREEN_WIDTH*0.5f-text_width)/2; rect.y = SCREEN_HEIGHT*0.5f;
  g_texture = load_texture("Sound_Setting.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  TTF_SizeText(g_font.font, "Indicator", &text_width, NULL);
  load_text("Indicator", colors[WHITE], (SCREEN_WIDTH*0.6f-text_width)/2, SCREEN_HEIGHT*0.65f, 255);
  rect.x = (SCREEN_WIDTH*0.5f-text_width)/2; rect.y = SCREEN_HEIGHT*0.65f;
  g_texture = load_texture("Indicator_Setting.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  SDL_DestroyTexture(off_button); SDL_DestroyTexture(on_button);
  TTF_SizeText(g_font.font, "Return", &text_width, NULL);
  g_texture = load_texture("Return_Button.png");
  (*button_rects)[RETURN] = (SDL_Rect) { (SCREEN_WIDTH-text_width)*0.465, SCREEN_HEIGHT*0.805, SCREEN_WIDTH*0.145, SCREEN_HEIGHT*0.1 };
  SDL_RenderCopy(g_renderer, g_texture, NULL, (*button_rects)+RETURN);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
  load_text("Return", colors[BLACK], (SCREEN_WIDTH-text_width)/2, SCREEN_HEIGHT*0.83, 255);
  SDL_RenderPresent(g_renderer);
}

void settings_return(Bool *quit_settings, Bool in_game, int *current_board, FloatTuple offset) {
  SDL_Delay(100);
  *quit_settings = TRUE;
  if (in_game == TRUE)
    draw(current_board, offset, TRUE);
  else {
    g_texture = load_texture("Main_Menu.png");
    SDL_RenderClear(g_renderer);
    SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
    show_menu_context(SETTINGS);
  }
}

void settings_screen(Bool *quit, Bool in_game, int *current_board, FloatTuple offset) {
  SDL_Delay(100);
  Bool quit_settings = FALSE, return_hover = FALSE;
  SDL_Rect *button_rects = malloc(sizeof(SDL_Rect)*SETTING_OPTION_COUNT);
  SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW), *hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
  show_settings_context(&button_rects);
  SDL_Event event;
  SettingOption current_hover = -1;
  while (quit_settings == FALSE && *quit == FALSE) {
    if (SDL_WaitEvent(&event)) {
      if (event.type == SDL_QUIT)
        *quit = TRUE;
      else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        settings_return(&quit_settings, in_game, current_board, offset);
      else if (event.type == SDL_MOUSEMOTION) {
        int x, y;
        Bool is_hovering = FALSE;
        SDL_GetMouseState(&x, &y);
        for (int i = 0; i < SETTING_OPTION_COUNT; i++) {
          if (x >= button_rects[i].x && x <= button_rects[i].x+button_rects[i].w && y >= button_rects[i].y && y <= button_rects[i].y+button_rects[i].h) {
            current_hover = i;
            SDL_SetCursor(hand_cursor);
            is_hovering = TRUE;
          }
        }
        if (is_hovering == FALSE) {
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
          if (fptr == NULL) { log_text("\tFailed to open file at <%s>. Screen settings were not saved.\n", path); }
          else {
            fprintf(fptr, "%d %d\nSCREEN_WIDTH SCREEN_HEIGHT\n", SCREEN_WIDTH, SCREEN_HEIGHT);
            fclose(fptr);
            log_text("\tScreen resolution changed to %dx%d. Screen Settings were saved correctly.\n\n", SCREEN_WIDTH, SCREEN_HEIGHT);
          }
          SDL_SetWindowSize(g_window, SCREEN_WIDTH, SCREEN_HEIGHT);
          SDL_SetWindowPosition(g_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
        else if (current_hover == SOUND_ON) { mute = FALSE; }
        else if (current_hover == SOUND_OFF) { mute = TRUE; }
        else if (current_hover == INDICATOR_ON) { indicator_on = TRUE; }
        else if (current_hover == INDICATOR_OFF) { indicator_on = FALSE; }
        else if (current_hover == RETURN) { settings_return(&quit_settings, in_game, current_board, offset); }
        if (current_hover != -1) {
          if (current_hover != SOUND_OFF) { play_sound(sounds[BIP2]); }
          if (current_hover != RETURN) { show_settings_context(&button_rects); }
        }
      }
    }
  }
  SDL_FreeCursor(arrow_cursor); SDL_FreeCursor(hand_cursor);
}

void show_menu_context(MenuHover current) {
  SDL_RenderPresent(g_renderer);
  SDL_DestroyTexture(g_texture);
  SDL_RenderClear(g_renderer);
  change_font_size(SCREEN_HEIGHT/20);
  SDL_Rect fill_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
  g_texture = load_texture("Main_Menu.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &fill_rect);
  SDL_DestroyTexture(g_texture);
  int width;
  char *texts[] = { "New Game", "Continue", "Settings", "Exit" };
  int y_pos[] = { SCREEN_HEIGHT*0.63, SCREEN_HEIGHT*0.72, SCREEN_HEIGHT*0.81, SCREEN_HEIGHT*0.9 };
  for (int i = 0; i < MENU_HOVER_COUNT; i++) {
    float alpha_mult = i == current ? 1 : 0.8;
    TTF_SizeText(g_font.font, texts[i], &width, NULL);
    load_text(texts[i], colors[WHITE], (SCREEN_WIDTH-width)/2, y_pos[i], 255*alpha_mult);
  }
  SDL_RenderPresent(g_renderer);
}

void new_game(int **current_board, Bool *game_over, Bool *start, FloatTuple offset) {
  SDL_Delay(150);
  log_text("\n\t---------- Started a New Round ----------\n\n");
  SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
  SDL_SetCursor(arrow_cursor);
  play_sound(sounds[BIP2]);
  current_player = WHITE_PLAYER;
  *game_over = FALSE;
  mandatory_pawn_move = FALSE;
  mandatory_queen_move = FALSE;
  shown = FALSE;
  *current_board = initialize_board();
  *start = TRUE;
  draw(*current_board, offset, TRUE);
  get_allowed_actions(*current_board);
  SDL_FreeCursor(arrow_cursor);
}

void continue_game(int **current_board, Bool *start, FloatTuple offset) {
  SDL_Delay(150);
  if (load_game(current_board, offset) == TRUE) {
    SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    SDL_SetCursor(arrow_cursor);
    play_sound(sounds[BIP2]);
    *start = TRUE;
    shown = FALSE;
    SDL_FreeCursor(arrow_cursor);
  }
  else
    play_sound(sounds[ERROR]);
}

void open_settings(int **current_board, Bool *quit, Bool *game_over, FloatTuple offset) {
  SDL_Delay(150);
  SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
  SDL_SetCursor(arrow_cursor);
  play_sound(sounds[BIP2]);
  settings_screen(quit, FALSE, *current_board, offset);
  if (quit == FALSE)
    main_menu(current_board, quit, game_over, offset);
  SDL_FreeCursor(arrow_cursor);
}

void exit_game(Bool *quit) {
  play_sound(sounds[BIP2]);
  SDL_Delay(100);
  *quit = TRUE;
}

void main_menu(int **current_board, Bool *quit, Bool *game_over, FloatTuple offset) {
  MenuHover current_hover = -1;
  show_menu_context(current_hover);
  SDL_Event event;
  SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW), *hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
  Bool start = FALSE;
  while (start == FALSE && *quit == FALSE) {
    if (SDL_WaitEvent(&event)) {
      if (event.type == SDL_QUIT)
        *quit = TRUE;
      else if (event.type == SDL_MOUSEMOTION) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        Bool is_hovering = FALSE;
        int text_width, text_height;
        char *texts[] = { "New Game", "Continue", "Settings", "Exit" };
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
            is_hovering = TRUE;
          }
        }
        if (current_hover != -1 && is_hovering == FALSE) {
          current_hover = -1;
          show_menu_context(-1);
          SDL_SetCursor(arrow_cursor);
        }
      }
      else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (current_hover == NEW_GAME)
          new_game(current_board, game_over, &start, offset);
        else if (current_hover == CONTINUE)
          continue_game(current_board, &start, offset);
        else if (current_hover == SETTINGS) {
          open_settings(current_board, quit, game_over, offset);
        }
        else if (current_hover == EXIT)
          exit_game(quit);
      }
      else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_DOWN:
            SDL_Delay(50);
            play_sound(sounds[BIP]);
            current_hover = (current_hover + 1) % MENU_HOVER_COUNT;
            show_menu_context(current_hover);
            break;
          case SDLK_UP:
            SDL_Delay(50);
            play_sound(sounds[BIP]);
            current_hover = (((current_hover - 1) % MENU_HOVER_COUNT) + MENU_HOVER_COUNT) % MENU_HOVER_COUNT;
            show_menu_context(current_hover);
            break;
          case SDLK_RETURN:
            if (current_hover == NEW_GAME)
              new_game(current_board, game_over, &start, offset);
            else if (current_hover == CONTINUE)
              continue_game(current_board, &start, offset);
            else if (current_hover == SETTINGS)
              open_settings(current_board, quit, game_over, offset);
            else if (current_hover == EXIT)
              exit_game(quit);
            break;
        }
      }
    }
  }
  SDL_FreeCursor(arrow_cursor); SDL_FreeCursor(hand_cursor);
}

Bool action_indicator(int index, int *current_board, FloatTuple offset) {
  get_allowed_piece_actions(index, current_board);
  if (current_board[index]/COLOR_DIVIDER == current_player) {
    if (indicator_on == TRUE) {
      IntTuple edge_map = map_to_edge(index, offset);
      FloatTuple cell_size = get_cell_size(index);
      SDL_Rect rect = { edge_map.x, edge_map.y, cell_size.x, cell_size.y };
      g_texture = load_texture("_Indicator.png");
      set_blend_mode(SDL_BLENDMODE_BLEND);
      set_alpha(120);
      SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
      SDL_DestroyTexture(g_texture);
      g_texture = NULL;
      g_texture = load_texture("Indicator.png");
      set_alpha(150);
      for (int i = 0; i < allowed_piece_count; i++) {
        edge_map = map_to_edge(current_allowed_piece[i], offset);
        cell_size = get_cell_size(current_allowed_piece[i]);
        rect = (SDL_Rect) { edge_map.x, edge_map.y, cell_size.x, cell_size.y };
        SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
      }
      SDL_DestroyTexture(g_texture);
      g_texture = NULL;
      SDL_RenderPresent(g_renderer);
    }
    return TRUE;
  }
  return FALSE;
}

void mandatory_indicator(int cell_index, FloatTuple offset, int grid_x, int grid_width) {
  if ((mandatory_pawn_move == TRUE || mandatory_queen_move == TRUE) && shown == FALSE) {
    SDL_Rect rects[allowed_count];
    for (int i = 0; i < allowed_count; i++) {
      if (cell_index == current_allowed[i].x) { return; }
      IntTuple edge_map = map_to_edge(current_allowed[i].x, offset);
      FloatTuple cell_size = get_cell_size(current_allowed[i].x);
      rects[i] = (SDL_Rect) { edge_map.x, edge_map.y, cell_size.x, cell_size.y };
    }
    int text_width;
    TTF_SizeText(g_font.font, "Mandatory Move", &text_width, NULL);
    load_text("Mandatory Move", colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.41, 255);
    play_sound(sounds[MANDATORY]);
    g_texture = load_texture("_Mandatory.png");
    set_blend_mode(SDL_BLENDMODE_BLEND);
    set_alpha(180);
    for (int i = 0; i < allowed_count; i++) { SDL_RenderCopy(g_renderer, g_texture, NULL, rects+i); }
    SDL_DestroyTexture(g_texture);
    g_texture = NULL;
  }
}

void hide_text(int grid_x, int grid_width) {
  SDL_Rect rect = { grid_x+grid_width*0.1, SCREEN_HEIGHT*0.55, grid_width*0.8, SCREEN_HEIGHT*0.05 };
  g_texture = load_texture("Hide_Text.png");
  SDL_RenderCopy(g_renderer, g_texture, NULL, &rect);
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;
}

void start_game(int **current_board) {
  init();
  load_all_sounds();
  load_font("bodoni_mtblack.ttf", SCREEN_HEIGHT/20);
  int x, y, text_width, last_active_cell = -1;
  FloatTuple offset = { SCREEN_HEIGHT*0.1f, 0 };
  Bool quit = FALSE, game_over = FALSE, log_shown = FALSE;
  SDL_Event event;
  main_menu(current_board, &quit, &game_over, offset);
  GameButton current_hover = -1;
  SDL_Cursor *arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW), *hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
  char *texts[] = { "Undo Move", "Save Game", "Load Game", "Settings", "Main Menu" };
  while (quit == FALSE) {
    float grid_x = SCREEN_HEIGHT+2*offset.x;
    float grid_width = SCREEN_WIDTH-(SCREEN_HEIGHT+2*offset.x);
    Bool is_hovering = FALSE;
    SDL_Rect b_rects[] = { { grid_x+grid_width*0.1f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f }, { grid_x+grid_width*0.275f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f }, { grid_x+grid_width*0.45f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f }, { grid_x+grid_width*0.625f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f }, { grid_x+grid_width*0.8f, SCREEN_HEIGHT*0.632f, SCREEN_HEIGHT/29.f*1.5f, SCREEN_HEIGHT/29.f*1.5f } };
    if (SDL_WaitEvent(&event)) {
      if (event.type == SDL_QUIT)
        quit = TRUE;
      else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          SDL_Delay(100);
          if (SDL_GetCursor() == hand_cursor)
            SDL_SetCursor(arrow_cursor);
          main_menu(current_board, &quit, &game_over, offset);
        }
      }
      else if (event.type == SDL_MOUSEMOTION) {
        SDL_GetMouseState(&x, &y);
        for (int i = 0; i < GAME_BUTTON_COUNT; i++) {
          if (x >= b_rects[i].x && x <= b_rects[i].x+b_rects[i].w && y >= b_rects[i].y && y <= b_rects[i].y+b_rects[i].h && log_shown == FALSE) {
            SDL_SetCursor(hand_cursor);
            TTF_SizeText(g_font.font, texts[i], &text_width, NULL);
            load_text(texts[i], colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.55, 255);
            SDL_RenderPresent(g_renderer);
            current_hover = i;
            is_hovering = TRUE;
          }
        }
        if (is_hovering == FALSE) {
          hide_text(grid_x, grid_width);
          SDL_SetCursor(arrow_cursor);
          current_hover = -1;
          log_shown = FALSE;
          SDL_RenderPresent(g_renderer);
        }
      }
      else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (current_hover == UNDO_MOVE_BUTTON)
          undo_last_move(current_board, &game_over, offset);
        else if (current_hover == SAVE_GAME_BUTTON) {
          Bool has_saved = save_game(*current_board, check_game_state(*current_board)); 
          if (has_saved == TRUE) {
            play_sound(sounds[BIP2]);
            hide_text(grid_x, grid_width);
            SDL_RenderPresent(g_renderer);
            TTF_SizeText(g_font.font, "Game Saved", &text_width, NULL);
            load_text("Game Saved", colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.55, 255);
            SDL_RenderPresent(g_renderer);
            log_shown = TRUE;
          }
          else {
            play_sound(sounds[ERROR]);
            hide_text(grid_x, grid_width);
            SDL_RenderPresent(g_renderer);
            TTF_SizeText(g_font.font, "Failed To Save", &text_width, NULL);
            load_text("Failed To Save", colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.55, 255);
            SDL_RenderPresent(g_renderer);
            log_shown = TRUE;
          }
        }
        else if (current_hover == LOAD_GAME_BUTTON) {
          Bool has_loaded = load_game(current_board, offset);
          if (has_loaded == TRUE) {
            play_sound(sounds[BIP2]);
            hide_text(grid_x, grid_width);
            SDL_RenderPresent(g_renderer);
            TTF_SizeText(g_font.font, "Game Loaded", &text_width, NULL);
            load_text("Game Loaded", colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.55, 255);
            SDL_RenderPresent(g_renderer);
            log_shown = TRUE;
          }
          else {
            play_sound(sounds[ERROR]);
            hide_text(grid_x, grid_width);
            SDL_RenderPresent(g_renderer);
            TTF_SizeText(g_font.font, "Failed To Load", &text_width, NULL);
            load_text("Failed To Load", colors[BLACK], grid_x+(grid_width-text_width)*0.5, SCREEN_HEIGHT*0.55, 255);
            SDL_RenderPresent(g_renderer);
            log_shown = TRUE;
          }
        }
        else if (current_hover == SETTINGS_BUTTON) {
          play_sound(sounds[BIP2]);
          settings_screen(&quit, TRUE, *current_board, offset);
        }
        else if (current_hover == MAIN_MENU_BUTTON) {
          play_sound(sounds[BIP2]);
          SDL_SetCursor(arrow_cursor);
          SDL_RenderClear(g_renderer);
          main_menu(current_board, &quit, &game_over, offset);
        }
        else if (current_hover == -1 && game_over == FALSE) {
          if (x >= offset.x && x <= SCREEN_HEIGHT+offset.x && y >= offset.y && y <= SCREEN_HEIGHT+offset.y) {
            int cell_index = map_to_int((IntTuple) { x-offset.x, y-offset.y });
            draw(*current_board, offset, FALSE); // Clears Highlighted Path
            if (last_active_cell == cell_index) {
              play_sound(sounds[MOVE2]);
              last_active_cell = -1;
            }
            else {
              Bool found = FALSE;
              if (last_active_cell != -1) {
                for (int i = 0; i < allowed_piece_count; i++) {
                  if (current_allowed_piece[i] == cell_index) {
                    found = TRUE;
                    GameState state = apply_move((IntTuple) { last_active_cell, cell_index }, current_board, offset);
                    game_over = state != ONGOING ? TRUE : FALSE;
                    game_over_screen(*current_board, state, grid_x, grid_width);
                    break;
                  }
                }
              }
              if (found == FALSE && last_active_cell != cell_index && action_indicator(cell_index, *current_board, offset) == TRUE) {
                mandatory_indicator(cell_index, offset, grid_x, grid_width);
                last_active_cell = cell_index;
                play_sound(sounds[MOVE]);
              }
              else {
                last_active_cell = -1;
              }
            }
          }
        }
      }
    }
  }
  SDL_FreeCursor(arrow_cursor); SDL_FreeCursor(hand_cursor);
  close();
}
