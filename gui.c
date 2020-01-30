#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "gui.h"
#include "move.h"
#include "init_mat.h"

int SCREEN_WIDTH;
int SCREEN_HEIGHT;
int size[2] = {0};
bool shown, mute = false, indicator_on = true;

SDL_Window *gWindow = NULL;
SDL_Surface *gSurface = NULL;
SDL_Texture *gTexture = NULL;
SDL_Renderer *gRenderer = NULL;
TTF_Font *gFont = NULL;
SDL_Color black = {50, 50, 50}, white = {255, 255, 255}, grey = {127, 127, 127}, light_grey = {241, 241, 241};
Mix_Chunk *sBip[2] = {NULL, NULL}, *sMove[2] = {NULL, NULL}, *sMandatory = NULL, *sPup = NULL, *sGameOver = NULL, *sError = NULL;

void init()
{
  FILE * fptr = fopen("screen_settings.txt", "r");
  fscanf(fptr, "%d %d", &SCREEN_WIDTH, &SCREEN_HEIGHT);
  fclose(fptr);
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
  {
    printf("Failed to initialize SDL!\n%s\n", SDL_GetError());
  }
  if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
  {
    printf("Failed to set hint!");
  }
  gWindow = SDL_CreateWindow("Bulltricker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if(gWindow == NULL)
  {
    printf("Failed to create window!\n%s\n", SDL_GetError());
  }
  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
  if(gRenderer == NULL)
  {
    printf("Failed to create renderer!\n%s\n", SDL_GetError());
  }
  SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
  if(!IMG_Init(IMG_INIT_PNG))
  {
    printf("Failed to initialize SDL_Image!\n%s\n", IMG_GetError());
  }
  if(TTF_Init() == -1)
  {
    printf("Failed to initialize SDL_TTF!\n%s\n", TTF_GetError());
  }
  if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
  {
    printf("Failed to initialize SDL_Mixer!\n%s\n", Mix_GetError());
  }
}

void loadSound()
{
  sBip[0] = Mix_LoadWAV("Sounds/menu_select.wav");
  sBip[1] = Mix_LoadWAV("Sounds/menu_click.wav");
  sMove[0] = Mix_LoadWAV("Sounds/move_1.wav");
  sMove[1] = Mix_LoadWAV("Sounds/move_2.wav");
  sMandatory = Mix_LoadWAV("Sounds/mandatory.wav");
  sPup = Mix_LoadWAV("Sounds/pawn_powerup.wav");
  sGameOver = Mix_LoadWAV("Sounds/game_over.wav");
  sError = Mix_LoadWAV("Sounds/error.wav");
}

void playSound(Mix_Chunk *sound)
{
  if(!mute)
  {
    Mix_PlayChannel(-1, sound, 0);
  }
}

void close()
{
  Mix_FreeChunk(sBip[0]);
  sBip[0] = NULL;
  Mix_FreeChunk(sBip[1]);
  sBip[1] = NULL;
  Mix_FreeChunk(sMove[0]);
  sMove[0] = NULL;
  Mix_FreeChunk(sMove[1]);
  sMove[1] = NULL;
  Mix_FreeChunk(sMandatory);
  sMandatory = NULL;
  TTF_CloseFont(gFont);
  gFont = NULL;
  SDL_DestroyRenderer(gRenderer);
  gRenderer = NULL;
  SDL_DestroyTexture(gTexture);
  gTexture = NULL;
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  Mix_Quit();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

void getSize(int x, int y)
{
  if((y/20) % 4 == 0)
  {
    if((x/20) % 4 == 0)
    {
      size[0] = 20;
      size[1] = 20;
    }
    else
    {
      size[0] = 60;
      size[1] = 20;
    }
  }
  else
  {
    if((x/20) % 4 == 0)
    {
      size[0] = 20;
      size[1] = 60;
    }
    else
    {
      size[0] = 60;
      size[1] = 60;
    }
  }
}

int mapToInt(int x)
{
  int edges[15] = {20, 80, 100, 160, 180, 240, 260, 320, 340, 400, 420, 480, 500, 560, 580};
  for(int i = 0; i < 15; i++)
  {
    if(x < edges[i])
    {
      return i;
    }
  }
}

int mapToEdge(int x)
{
  int edges[15] = {0, 20, 80, 100, 160, 180, 240, 260, 320, 340, 400, 420, 480, 500, 560};
  return edges[x];
}

TTF_Font *loadFont(char *path, unsigned char size)
{
  TTF_Font *newFont = TTF_OpenFont(path, size);
  if(newFont == NULL)
  {
    printf("Failed to load font %s!\n%s\n", path, TTF_GetError());
  }
  return newFont;
}

SDL_Texture *loadTexture(char *path)
{
  SDL_Texture *newTexture = NULL;
  newTexture = IMG_LoadTexture(gRenderer, path);
  if(newTexture == NULL)
  {
    printf("Failed to create texture from surface %s!\n%s\n", path, IMG_GetError());
  }
  return newTexture;
}

void loadText(char *text, SDL_Color color, int x, int y, int width, int height)
{
  SDL_Rect textRect = {x, y, width, height};
  gSurface = TTF_RenderText_Blended(gFont, text, color);
  gTexture = SDL_CreateTextureFromSurface(gRenderer, gSurface);
  SDL_FreeSurface(gSurface);
  gSurface = NULL;
  SDL_RenderCopy(gRenderer, gTexture, NULL, &textRect);
  SDL_DestroyTexture(gTexture);
  gTexture = NULL;
}

void loadGrid()
{
  SDL_Delay(50);
  SDL_Rect fillRect = {600, 0, 200, 580};
  gTexture = loadTexture("Sprites/Grid.png");
  SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
  SDL_DestroyTexture(gTexture);
  gTexture = NULL;
  SDL_Texture *buttonList[4] = {loadTexture("Sprites/Undo_Button.png"), loadTexture("Sprites/Save_Button.png"), loadTexture("Sprites/Load_Button.png"), loadTexture("Sprites/Menu_Button.png")};
  SDL_Rect fillRect1 = {617, 360, 30, 30};
  SDL_Rect fillRect2 = {662, 360, 30, 30};
  SDL_Rect fillRect3 = {707, 360, 30, 30};
  SDL_Rect fillRect4 = {752, 360, 30, 30};
  gTexture = buttonList[0];
  SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect1);
  gTexture = buttonList[1];
  SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect2);
  gTexture = buttonList[2];
  SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect3);
  gTexture = buttonList[3];
  SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect4);
  for(int i = 0; i < 4; i++)
  {
    SDL_DestroyTexture(buttonList[i]);
    buttonList[i] = NULL;
  }
  char *text;
  if(actual_player % 2 == 0)
  {
    text = "Whites turn";
  }
  else
  {
    text = "Blacks turn";
  }
  loadText(text, black, 660, 190, 120, 30);
  if(queen_mandatory || pawn_mandatory)
  {
    loadText("Obligatory move", black, 630, 240, 140, 30);
  }
}

void fillBlank()
{
  SDL_RenderClear(gRenderer);
  SDL_Rect fillRect1 = {580, 0, 20, 580};
  SDL_Rect fillRect2 = {0, 580, 800, 20};
  SDL_Texture *newTexture = loadTexture("Sprites/fill1.png");
  SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(newTexture, 200);
  SDL_RenderCopy(gRenderer, newTexture, NULL, &fillRect1);
  SDL_DestroyTexture(newTexture);
  newTexture = loadTexture("Sprites/fill2.png");
  SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(newTexture, 200);
  SDL_RenderCopy(gRenderer, newTexture, NULL, &fillRect2);
  SDL_DestroyTexture(newTexture);
  newTexture = NULL;
}

void draw(int **actual_matrix)
{
  fillBlank();
  loadGrid();
  int x, y;
  for(int i = 0, k = 0; i < 580, k < 15; k++)
  {
    for(int j = 0, l = 0; j < 580, l < 15; l++)
    {
      if(actual_matrix[l][k] == -1)
      {
        gTexture = loadTexture("Sprites/NP.png");
        SDL_Rect fillRect = {i, j, 20, 20};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=20;
        y=20;
      }
      else if(actual_matrix[l][k] == 0 && l % 2 == 1)
      {
        gTexture = loadTexture("Sprites/P0.png");
        SDL_Rect fillRect = {i, j, 20, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=20;
        y=60;
      }
      else if(actual_matrix[l][k] == 0 && l % 2 == 0)
      {
        gTexture = loadTexture("Sprites/P1.png");
        SDL_Rect fillRect = {i, j, 60, 20};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=20;
      }
      else if(actual_matrix[l][k] == -2)
      {
        gTexture = loadTexture("Sprites/PK.png");
        SDL_Rect fillRect = {i, j, 60, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=60;
      }
      else if(actual_matrix[l][k] == 13)
      {
        gTexture = loadTexture("Sprites/KB.png");
        SDL_Rect fillRect = {i, j, 60, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=60;
      }
      else if(actual_matrix[l][k] == 23)
      {
        gTexture = loadTexture("Sprites/KW.png");
        SDL_Rect fillRect = {i, j, 60, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=60;
      }
      else if(actual_matrix[l][k] == 121)
      {
        gTexture = loadTexture("Sprites/QBR.png");
        SDL_Rect fillRect = {i, j, 60, 20};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=20;
      }
      else if(actual_matrix[l][k] == 120)
      {
        gTexture = loadTexture("Sprites/QB.png");
        SDL_Rect fillRect = {i, j, 20, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=20;
        y=60;
      }
      else if(actual_matrix[l][k] == 221)
      {
        gTexture = loadTexture("Sprites/QWR.png");
        SDL_Rect fillRect = {i, j, 60, 20};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=20;
      }
      else if(actual_matrix[l][k] == 220)
      {
        gTexture = loadTexture("Sprites/QW.png");
        SDL_Rect fillRect = {i, j, 20, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=20;
        y=60;
      }
      else if(actual_matrix[l][k] == 110)
      {
        gTexture = loadTexture("Sprites/PBR.png");
        SDL_Rect fillRect = {i, j, 20, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=20;
        y=60;
      }
      else if(actual_matrix[l][k] == 210)
      {
        gTexture = loadTexture("Sprites/PWR.png");
        SDL_Rect fillRect = {i, j, 20, 60};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=20;
        y=60;
      }
      else if(actual_matrix[l][k] == 111)
      {
        gTexture = loadTexture("Sprites/PB.png");
        SDL_Rect fillRect = {i, j, 60, 20};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=20;
      }
      else if(actual_matrix[l][k] == 211)
      {
        gTexture = loadTexture("Sprites/PW.png");
        SDL_Rect fillRect = {i, j, 60, 20};
        SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
        x=60;
        y=20;
      }
      j+=y;
      SDL_DestroyTexture(gTexture);
      gTexture = NULL;
    }
    i+=x;
  }
  SDL_RenderPresent(gRenderer);
}

void saveGame(int **actual_matrix, bool game_over)
{
  FILE *fptr = fopen("Saves/save.bin", "wb");
  for(int i = 0; i < 15; i++)
  {
    for(int j = 0; j < 15; j++)
    {
      fwrite(&actual_matrix[i][j], sizeof(int), 1, fptr);
    }
  }
  fwrite(&actual_player, sizeof(int), 1, fptr);
  fwrite(&game_over, sizeof(bool), 1, fptr);
  fclose(fptr);
}

bool loadGame(int ***actual_matrix, bool game_over)
{
  *actual_matrix = initMat();
  FILE *fptr = fopen("Saves/save.bin", "rb");
  if(fptr != NULL)
  {
    for(int i = 0; i < 15; i++)
    {
      for(int j = 0; j < 15; j++)
      {
        fread(&((*actual_matrix)[i][j]), sizeof(int), 1, fptr);
      }
    }
    fread(&actual_player, sizeof(int), 1, fptr);
    fread(&game_over, sizeof(bool), 1, fptr);
    fclose(fptr);
    return true;
  }
  return false;
}

void gameOver(int **actual_matrix, bool *game_over)
{
  if(check_mat(actual_matrix) == whites)
  {
    playSound(sGameOver);
    *game_over = true;
    char *text = "Game Over";
    loadText(text, black, 640, 240, 120, 30);
    text = "Player 1 Wins";
    loadText(text, black, 620, 280, 160, 30);
  }
  else if(check_mat(actual_matrix) == blacks)
  {
    playSound(sGameOver);
    *game_over = true;
    char *text = "Game Over";
    loadText(text, black, 640, 240, 120, 30);
    text = "Player 2 Wins";
    loadText(text, black, 620, 280, 160, 30);
  }
  else if(check_null(actual_matrix))
  {
    playSound(sGameOver);
    *game_over = true;
    char *text = "Game Over";
    loadText(text, black, 640, 240, 120, 30);
    text = "It's a tie";
    loadText(text, black, 640, 280, 120, 30);
  }
  SDL_RenderPresent(gRenderer);
}

void introScreen()
{
  gTexture = loadTexture("Sprites/intro_screen.png");
  for(int i = 0; i < 256; i++)
  {
    SDL_SetTextureAlphaMod(gTexture, i);
    SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
    SDL_RenderPresent(gRenderer);
  }
  SDL_DestroyTexture(gTexture);
  gTexture = NULL;
  SDL_RenderClear(gRenderer);
}

void settingScreen(bool *quit)
{
  SDL_Delay(100);
  bool quit_settings = false, sound_hover = false, indicator_hover = false, return_hover = false;
  SDL_RenderPresent(gRenderer);
  gTexture = loadTexture("Sprites/Menu_Settings.png");
  SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
  SDL_DestroyTexture(gTexture);
  gTexture = NULL;
  loadText("Settings", white, 340, 60, 120, 40);
  SDL_Texture *buttonList[15] = {loadTexture("Sprites/Sound_On.png"), loadTexture("Sprites/Sound_On_Hover.png"), loadTexture("Sprites/Sound_On_Click.png"), loadTexture("Sprites/Sound_Off.png"), loadTexture("Sprites/Sound_Off_Hover.png"), loadTexture("Sprites/Sound_Off_Click.png"), loadTexture("Sprites/Indicator_On.png"), loadTexture("Sprites/Indicator_On_Hover.png"), loadTexture("Sprites/Indicator_On_Click.png"), loadTexture("Sprites/Indicator_Off.png"), loadTexture("Sprites/Indicator_Off_Hover.png"), loadTexture("Sprites/Indicator_Off_Click.png"), loadTexture("Sprites/return_button.png"), loadTexture("Sprites/return_button_hover.png"), loadTexture("Sprites/return_button_click.png")};
  SDL_Rect soundButtonRect = {400 - 138/2, 200, 138, 46};
  SDL_Rect soundButtonRectClick = {400 - 140/2, 199, 140, 48};
  SDL_Rect indicatorButtonRect = {400 - 138/2, 300, 138, 46};
  SDL_Rect indicatorButtonRectClick = {400 - 140/2, 299, 140, 48};
  SDL_Rect returnButtonRect = {1, 1, 52, 46};
  SDL_Rect returnButtonRectClick = {0, 0, 54, 48};
  gTexture = buttonList[12];
  SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
  if(!mute)
  {
    gTexture = buttonList[0];
  }
  else
  {
    gTexture = buttonList[3];
  }
  SDL_RenderCopy(gRenderer, gTexture, NULL, &soundButtonRect);
  if(indicator_on)
  {
    gTexture = buttonList[6];
  }
  else if(!indicator_on)
  {
    gTexture = buttonList[9];
  }
  SDL_RenderCopy(gRenderer, gTexture, NULL, &indicatorButtonRect);
  SDL_RenderPresent(gRenderer);
  SDL_Event event;
  while(!quit_settings && !*quit)
  {
    if(SDL_WaitEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        *quit = true;
      }
      else if(event.type == SDL_KEYDOWN)
      {
        if(event.key.keysym.sym == SDLK_ESCAPE)
        {
          SDL_Delay(100);
          quit_settings = true;
          for(int i = 0; i < 15; i++)
          {
            SDL_DestroyTexture(buttonList[i]);
            buttonList[i] = NULL;
          }
          SDL_DestroyTexture(gTexture);
          gTexture = loadTexture("Sprites/Main_Menu_0.png");
          SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
          SDL_DestroyTexture(gTexture);
          gTexture = NULL;
          SDL_RenderPresent(gRenderer);
        }
      }
      else if(event.type == SDL_MOUSEMOTION)
      {
        int x,y;
        SDL_GetMouseState(&x, &y);
        if(x >= 400 - 138/2 && y >= 200 && x <= 400 + 138/2 && y <= 246 && !sound_hover)
        {
          sound_hover = true;
          if(!mute)
          {
            gTexture = buttonList[1];
          }
          else
          {
            gTexture = buttonList[4];
          }
          SDL_RenderCopy(gRenderer, gTexture, NULL, &soundButtonRect);
          SDL_RenderPresent(gRenderer);
        }
        else if(sound_hover && (x < 400 - 138/2 || y < 200 || x > 400 + 138/2 || y > 246))
        {
          sound_hover = false;
          if(!mute)
          {
            gTexture = buttonList[0];
          }
          else
          {
            gTexture = buttonList[3];
          }
          SDL_RenderCopy(gRenderer, gTexture, NULL, &soundButtonRect);
          SDL_RenderPresent(gRenderer);
        }
        else if(x >= 400 - 138/2 && y >= 300 && x <= 400 + 138/2 && y <= 346 && !indicator_hover)
        {
          indicator_hover = true;
          if(indicator_on)
          {
            gTexture = buttonList[7];
          }
          else
          {
            gTexture = buttonList[10];
          }
          SDL_RenderCopy(gRenderer, gTexture, NULL, &indicatorButtonRect);
          SDL_RenderPresent(gRenderer);
        }
        else if(indicator_hover && (x < 400 - 138/2 || y < 300 || x > 400 + 138/2 || y > 346))
        {
          indicator_hover = false;
          if(indicator_on)
          {
            gTexture = buttonList[6];
          }
          else
          {
            gTexture = buttonList[9];
          }
          SDL_RenderCopy(gRenderer, gTexture, NULL, &indicatorButtonRect);
          SDL_RenderPresent(gRenderer);
        }
        else if(!return_hover && x <= 53 && y <= 47)
        {
          return_hover = true;
          gTexture = buttonList[13];
          SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
          SDL_RenderPresent(gRenderer);
        }
        else if(return_hover && (x > 53 || y > 47))
        {
          return_hover = false;
          gTexture = buttonList[12];
          SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
          SDL_RenderPresent(gRenderer);
        }
      }
      else if(event.type == SDL_MOUSEBUTTONDOWN)
      {
        int x,y;
        SDL_GetMouseState(&x, &y);
        if(x >= 400 - 140/2 && y >= 199 && x <= 400 + 140/2 && y <= 247)
        {
          playSound(sBip[1]);
          if(!mute)
          {
            gTexture = buttonList[2];
          }
          else
          {
            gTexture = buttonList[5];
          }
          SDL_RenderCopy(gRenderer, gTexture, NULL, &soundButtonRectClick);
          SDL_RenderPresent(gRenderer);
          if(SDL_WaitEvent(&event) && event.type == SDL_MOUSEBUTTONUP)
          {
            sound_hover = false;
            SDL_Delay(50);
            SDL_RenderClear(gRenderer);
            gTexture = loadTexture("Sprites/Menu_Settings.png");
            SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
            SDL_DestroyTexture(gTexture);
            loadText("Settings", white, 340, 60, 120, 40);
            gTexture = buttonList[12];
            SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
            if(!mute)
            {
              gTexture = buttonList[3];
              mute = true;
            }
            else
            {
              gTexture = buttonList[0];
              mute = false;
            }
            SDL_RenderCopy(gRenderer, gTexture, NULL, &soundButtonRect);
            if(indicator_on)
            {
              gTexture = buttonList[6];
            }
            else
            {
              gTexture = buttonList[9];
            }
            SDL_RenderCopy(gRenderer, gTexture, NULL, &indicatorButtonRect);
            SDL_RenderPresent(gRenderer);
          }
        }
        else if(x >= 400 - 140/2 && y >= 300 && x <= 400 + 140/2 && y <= 346)
        {
          playSound(sBip[1]);
          if(indicator_on)
          {
            gTexture = buttonList[8];
          }
          else
          {
            gTexture = buttonList[11];
          }
          SDL_RenderCopy(gRenderer, gTexture, NULL, &indicatorButtonRectClick);
          SDL_RenderPresent(gRenderer);
          if(SDL_WaitEvent(&event) && event.type == SDL_MOUSEBUTTONUP)
          {
            indicator_hover = false;
            SDL_Delay(50);
            SDL_RenderClear(gRenderer);
            gTexture = loadTexture("Sprites/Menu_Settings.png");
            SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
            SDL_DestroyTexture(gTexture);
            loadText("Settings", white, 340, 60, 120, 40);
            gTexture = buttonList[12];
            SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
            if(indicator_on)
            {
              gTexture = buttonList[9];
              indicator_on = false;
            }
            else
            {
              gTexture = buttonList[6];
              indicator_on = true;
            }
            SDL_RenderCopy(gRenderer, gTexture, NULL, &indicatorButtonRect);
            if(!mute)
            {
              gTexture = buttonList[0];
            }
            else
            {
              gTexture = buttonList[3];
            }
            SDL_RenderCopy(gRenderer, gTexture, NULL, &soundButtonRect);
            SDL_RenderPresent(gRenderer);
          }
        }
        else if(return_hover)
        {
          playSound(sBip[1]);
          gTexture = buttonList[14];
          SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRectClick);
          SDL_RenderPresent(gRenderer);
          if(SDL_WaitEvent(&event) && event.type == SDL_MOUSEBUTTONUP)
          {
            gTexture = buttonList[12];
            SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
            SDL_RenderPresent(gRenderer);
            SDL_Delay(100);
            quit_settings = true;
            for(int i = 0; i < 15; i++)
            {
              SDL_DestroyTexture(buttonList[i]);
              buttonList[i] = NULL;
            }
            SDL_DestroyTexture(gTexture);
            gTexture = loadTexture("Sprites/Main_Menu_0.png");
            SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
            SDL_DestroyTexture(gTexture);
            gTexture = NULL;
            SDL_RenderPresent(gRenderer);
          }
        }
      }
    }
  }
}

void instructionScreen(bool *quit)
{
  SDL_Delay(100);
  bool quit_instruct = false, return_hover = false;
  SDL_Event event;
  SDL_Texture *instructionScreen = loadTexture("Sprites/Instructions.png");
  SDL_Texture *buttonList[3] = {loadTexture("Sprites/return_button.png"), loadTexture("Sprites/return_button_hover.png"), loadTexture("Sprites/return_button_click.png")};
  gTexture = instructionScreen;
  SDL_Rect returnButtonRect = {1, 1, 52, 46};
  SDL_Rect returnButtonRectClick = {0, 0, 54, 48};
  SDL_Rect scrollRect = {0, 0, 800, 580};
  SDL_Rect fillRect = {0, 0, 800, 580};
  SDL_RenderCopy(gRenderer, gTexture, &scrollRect, &fillRect);
  gTexture = buttonList[0];
  SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
  SDL_RenderPresent(gRenderer);
  while(!quit_instruct && !*quit)
  {
    if(SDL_WaitEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        *quit = true;
      }
      else if(event.type == SDL_KEYDOWN)
      {
        if(event.key.keysym.sym == SDLK_ESCAPE)
        {
          SDL_Delay(100);
          quit_instruct = true;
          for(int i = 0; i < 3; i++)
          {
            SDL_DestroyTexture(buttonList[i]);
            buttonList[i] = NULL;
          }
          SDL_DestroyTexture(instructionScreen);
          instructionScreen = NULL;
          SDL_DestroyTexture(gTexture);
          gTexture = loadTexture("Sprites/Main_Menu_0.png");
          SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
          SDL_DestroyTexture(gTexture);
          gTexture = NULL;
          SDL_RenderPresent(gRenderer);
        }
        else if(event.key.keysym.sym == SDLK_UP)
        {
          SDL_RenderClear(gRenderer);
          scrollRect.y -= 50;
          if(scrollRect.y < 0)
          {
            scrollRect.y = 0;
          }
          gTexture = instructionScreen;
          SDL_RenderCopy(gRenderer, gTexture, &scrollRect, &fillRect);
          if(scrollRect.y < 40)
          {
            gTexture = buttonList[0];
            SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
          }
          SDL_RenderPresent(gRenderer);
        }
        else if(event.key.keysym.sym == SDLK_DOWN)
        {
          SDL_RenderClear(gRenderer);
          scrollRect.y += 50;
          if(scrollRect.y > 1616)
          {
            scrollRect.y = 1616;
          }
          gTexture = instructionScreen;
          SDL_RenderCopy(gRenderer, gTexture, &scrollRect, &fillRect);
          if(scrollRect.y < 40)
          {
            gTexture = buttonList[0];
            SDL_Rect Rect = {0, scrollRect.y, 52, 46 - scrollRect.y};
            SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
          }
          SDL_RenderPresent(gRenderer);
        }
      }
      else if(event.type == SDL_MOUSEMOTION)
      {
        int i, j;
        SDL_GetMouseState(&i, &j);
        if(!return_hover && i <= 53 && j <= 47 && scrollRect.y < 40)
        {
          return_hover = true;
          gTexture = buttonList[1];
          SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
          SDL_RenderPresent(gRenderer);
        }
        else if(return_hover && (i > 53 || j > 47) && scrollRect.y < 40)
        {
          return_hover = false;
          gTexture = buttonList[0];
          SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
          SDL_RenderPresent(gRenderer);
        }
      }
      else if(event.type == SDL_MOUSEWHEEL)
      {
        if(event.wheel.y > 0)
        {
          SDL_RenderClear(gRenderer);
          scrollRect.y -= event.wheel.y * 10;
          if(scrollRect.y < 0)
          {
            scrollRect.y = 0;
          }
          gTexture = instructionScreen;
          SDL_RenderCopy(gRenderer, gTexture, &scrollRect, &fillRect);
          if(scrollRect.y < 40)
          {
            gTexture = buttonList[0];
            SDL_Rect Rect = {0, scrollRect.y, 52, 46 - scrollRect.y};
            SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
          }
          SDL_RenderPresent(gRenderer);
        }
        else if(event.wheel.y < 0)
        {
          SDL_RenderClear(gRenderer);
          scrollRect.y -= event.wheel.y * 10;
          if(scrollRect.y > 1616)
          {
            scrollRect.y = 1616;
          }
          gTexture = instructionScreen;
          SDL_RenderCopy(gRenderer, gTexture, &scrollRect, &fillRect);
          if(scrollRect.y < 40)
          {
            gTexture = buttonList[0];
            SDL_Rect Rect = {0, scrollRect.y, 52, 46 - scrollRect.y};
            SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
          }
          SDL_RenderPresent(gRenderer);
        }
      }
      else if(return_hover)
      {
        playSound(sBip[1]);
        gTexture = buttonList[2];
        SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRectClick);
        SDL_RenderPresent(gRenderer);
        if(SDL_WaitEvent(&event) && event.type == SDL_MOUSEBUTTONUP)
        {
          gTexture = buttonList[0];
          SDL_RenderCopy(gRenderer, gTexture, NULL, &returnButtonRect);
          SDL_RenderPresent(gRenderer);
          SDL_Delay(100);
          quit_instruct = true;
          for(int i = 0; i < 3; i++)
          {
            SDL_DestroyTexture(buttonList[i]);
            buttonList[i] = NULL;
          }
          SDL_DestroyTexture(gTexture);
          SDL_DestroyTexture(instructionScreen);
          instructionScreen = NULL;
          gTexture = loadTexture("Sprites/Main_Menu_0.png");
          SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
          SDL_DestroyTexture(gTexture);
          gTexture = NULL;
          SDL_RenderPresent(gRenderer);
        }
      }
    }
  }
}

void mainMenu(int ***actual_matrix, bool *game_over, bool *quit, int *move)
{
  SDL_RenderPresent(gRenderer);
  SDL_RenderClear(gRenderer);
  SDL_Rect fillRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  SDL_Texture *textureList[5] = {loadTexture("Sprites/Main_Menu_1.png"), loadTexture("Sprites/Main_Menu_2.png"), loadTexture("Sprites/Main_Menu_3.png"), loadTexture("Sprites/Main_Menu_4.png"), loadTexture("Sprites/Main_Menu_0.png")};
  int i = 4;
  gTexture = textureList[i];
  SDL_SetTextureAlphaMod(gTexture, 255);
  SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
  SDL_RenderPresent(gRenderer);
  SDL_Event event;
  bool start = false;
  while(!start && !*quit)
  {
    if(SDL_WaitEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        *quit = true;
      }
      else if(event.type == SDL_MOUSEMOTION)
      {
        int x, y;
        SDL_GetMouseState(&x, &y);
        if(y >= 375 && y < 420 && i != 0)
        {
          SDL_Delay(50);
          playSound(sBip[0]);
          i = 0;
          gTexture = textureList[i];
          SDL_RenderClear(gRenderer);
          SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
          SDL_RenderPresent(gRenderer);
        }
        else if(y >= 420 && y < 475 && i != 1)
        {
          SDL_Delay(50);
          playSound(sBip[0]);
          i = 1;
          gTexture = textureList[i];
          SDL_RenderClear(gRenderer);
          SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
          SDL_RenderPresent(gRenderer);
        }
        else if(y >= 475 && y < 525 && i != 2)
        {
          SDL_Delay(50);
          playSound(sBip[0]);
          i = 2;
          gTexture = textureList[i];
          SDL_RenderClear(gRenderer);
          SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
          SDL_RenderPresent(gRenderer);
        }
        else if(y >= 525 && i != 3)
        {
          SDL_Delay(50);
          playSound(sBip[0]);
          i = 3;
          gTexture = textureList[i];
          SDL_RenderClear(gRenderer);
          SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
          SDL_RenderPresent(gRenderer);
        }
      }
      else if(event.type == SDL_MOUSEBUTTONDOWN)
      {
        int x, y;
        SDL_GetMouseState(&x, &y);
        if(i == 0 && y >= 375 && y < 420)
        {
          SDL_Delay(150);
          playSound(sBip[1]);
          *move = 0;
          actual_player = 0;
          *game_over = false;
          queen_mandatory = false;
          pawn_mandatory = false;
          shown = false;
          *actual_matrix = initMat();
          start = true;
          SDL_DestroyTexture(gTexture);
          for(i = 0; i < 4; i++)
          {
            SDL_DestroyTexture(textureList[i]);
            textureList[i] = NULL;
          }
          gTexture = NULL;
          draw(*actual_matrix);
        }
        else if(i == 1 && y >= 420 && y < 475)
        {
          SDL_Delay(150);
          if(loadGame(actual_matrix, *game_over) == true)
          {
            playSound(sBip[1]);
            start = true;
            shown = false;
            SDL_DestroyTexture(gTexture);
            for(i = 0; i < 4; i++)
            {
              SDL_DestroyTexture(textureList[i]);
              textureList[i] = NULL;
            }
            gTexture = NULL;
            draw(*actual_matrix);
            gameOver(*actual_matrix, game_over);
          }
          else
          {
            playSound(sError);
          }
        }
        else if(i == 2 && y >= 475 && y < 525)
        {
          playSound(sBip[1]);
          settingScreen(quit);
          if(!quit)
          {
            for(i = 0; i < 4; i++)
            {
              SDL_DestroyTexture(textureList[i]);
              textureList[i] = NULL;
            }
            gTexture = NULL;
            mainMenu(actual_matrix, game_over, quit, move);
          }
        }
        else if(i == 3 && y >= 525)
        {
          playSound(sBip[1]);
          instructionScreen(quit);
          if(!quit)
          {
            for(i = 0; i < 4; i++)
            {
              SDL_DestroyTexture(textureList[i]);
              textureList[i] = NULL;
            }
            gTexture = NULL;
            mainMenu(actual_matrix, game_over, quit, move);
          }
        }
      }
      else if(event.type == SDL_KEYDOWN)
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_DOWN:
          {
            SDL_Delay(50);
            playSound(sBip[0]);
            if(i != 4)
            {
              i++;
              i = i % 4;
            }
            else
            {
              i = 0;
            }
            gTexture = textureList[i];
            SDL_RenderClear(gRenderer);
            SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
            SDL_RenderPresent(gRenderer);
            break;
          }
          case SDLK_UP:
          {
            SDL_Delay(50);
            playSound(sBip[0]);
            i--;
            if(i == -1)
            {
              i = 3;
            }
            gTexture = textureList[i];
            SDL_RenderClear(gRenderer);
            SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
            SDL_RenderPresent(gRenderer);
            break;
          }
          case SDLK_RETURN:
          {
            if(i == 0)
            {
              SDL_Delay(150);
              playSound(sBip[1]);
              *move = 0;
              actual_player = 0;
              *game_over = false;
              queen_mandatory = false;
              pawn_mandatory = false;
              shown = false;
              start = true;
              SDL_DestroyTexture(gTexture);
              for(i = 0; i < 4; i++)
              {
                SDL_DestroyTexture(textureList[i]);
                textureList[i] = NULL;
              }
              gTexture = NULL;
              draw(*actual_matrix);
            }
            else if(i == 1)
            {
              SDL_Delay(150);
              playSound(sBip[1]);
              if(loadGame(actual_matrix, *game_over) == true)
              {
                start = true;
                SDL_DestroyTexture(gTexture);
                for(i = 0; i < 4; i++)
                {
                  SDL_DestroyTexture(textureList[i]);
                  textureList[i] = NULL;
                }
                gTexture = NULL;
                draw(*actual_matrix);
                gameOver(*actual_matrix, game_over);
              }
            }
            else if(i == 2)
            {
              playSound(sBip[1]);
              settingScreen(quit);
              if(!quit)
              {
                for(i = 0; i < 4; i++)
                {
                  SDL_DestroyTexture(textureList[i]);
                  textureList[i] = NULL;
                }
                gTexture = NULL;
                mainMenu(actual_matrix, game_over, quit, move);
              }
            }
            else if(i == 3)
            {
              playSound(sBip[1]);
              instructionScreen(quit);
              if(!quit)
              {
                for(i = 0; i < 4; i++)
                {
                  SDL_DestroyTexture(textureList[i]);
                  textureList[i] = NULL;
                }
                gTexture = NULL;
                mainMenu(actual_matrix, game_over, quit, move);
              }
            }
            break;
          }
        }
      }
    }
  }
}

void setBlendMode(SDL_BlendMode blendMode)
{
  SDL_SetTextureBlendMode(gTexture, blendMode);
}

void setAlpha(unsigned char alpha)
{
  SDL_SetTextureAlphaMod(gTexture, alpha);
}

bool indicator(int x, int y, int **actual_matrix)
{
  if(indicator_on && ((actual_player % 2 == 0 && (actual_matrix[y][x] / 100 == 2 || actual_matrix[y][x] / 10 == 2)) || (actual_player % 2 == 1 && (actual_matrix[y][x] / 100 == 1 || actual_matrix[y][x] / 10 == 1))))
  {
    if(!queen_mandatory && !pawn_mandatory && x != mandatory_x && y != mandatory_y)
    {
      SDL_Delay(50);
      int temp_x = mapToEdge(x);
      int temp_y = mapToEdge(y);
      SDL_Rect fillRect = {temp_x, temp_y, size[0], size[1]};
      gTexture = loadTexture("Sprites/_Indicator.png");
      setBlendMode(SDL_BLENDMODE_BLEND);
      setAlpha(120);
      SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
      SDL_DestroyTexture(gTexture);
      gTexture = NULL;
    }
    gTexture = loadTexture("Sprites/Indicator.png");
    setAlpha(150);
    for(int u = 0; u < 15; u++)
    {
      for(int v = 0; v < 15; v++)
      {
        if(check_move(y, x, u, v, actual_matrix) != impossible)
        {
          int temp_u = mapToEdge(u);
          int temp_v = mapToEdge(v);
          getSize(temp_u, temp_v);
          SDL_Rect _fillRect = {temp_v, temp_u, size[1], size[0]};
          SDL_RenderCopy(gRenderer, gTexture, NULL, &_fillRect);
        }
      }
    }
    SDL_DestroyTexture(gTexture);
    gTexture = NULL;
    SDL_RenderPresent(gRenderer);
    SDL_RenderClear(gRenderer);
    return true;
  }
  return false;
}

void mandatoryIndicator()
{
  if((queen_mandatory || pawn_mandatory) && !shown)
  {
    int temp_x = mapToEdge(mandatory_y);
    int temp_y = mapToEdge(mandatory_x);
    getSize(temp_x, temp_y);
    SDL_Rect fillRect = {temp_x, temp_y, size[0], size[1]};
    gTexture = loadTexture("Sprites/_Mandatory.png");
    setBlendMode(SDL_BLENDMODE_BLEND);
    setAlpha(180);
    SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
    SDL_DestroyTexture(gTexture);
    gTexture = NULL;
    SDL_RenderPresent(gRenderer);
    shown = true;
  }
}

// To-do: -- Rework UI ...
//        -- Clean code ...
//        -- Implement VS AI mode ...
//        -- Host a local network game ...
//        -- Move on to Unity/Unreal Engine for further development ...

void createBoard(int ***actual_matrix)
{
  init();
  loadSound();
  gFont = loadFont("Fonts/bodoni_mtblack.ttf", 40);
  int x, y, u, v, move = 0, p[3];
  bool quit = false, game_over = false, drawn = false, restart;
  SDL_Event event;
  introScreen();
  mainMenu(actual_matrix, &game_over, &quit, &move);
  while(!quit)
  {
    if(SDL_WaitEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        quit = true;
      }
      else if(event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN)
      {
        if(move % 2 != 1)
        {
          SDL_GetMouseState(&x, &y);
        }
        if(x > 580 && y >= 360 && y < 391)
        {
          if(x >= 617 && x < 648)
          {
            if(!drawn)
            {
              SDL_Delay(100);
              loadText("Restart Game", black, 630, 318, 140, 30);
              SDL_RenderPresent(gRenderer);
              drawn = true;
            }
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
              playSound(sBip[1]);
              restart = false;
              for(int i = 0; i < 15; i++)
              {
                for(int j = 0; j < 15; j++)
                {
                  if((*actual_matrix)[i][j] != copy_matrix[i][j])
                  {
                    move = 0;
                    actual_player = 0;
                    game_over = false;
                    queen_mandatory = false;
                    pawn_mandatory = false;
                    *actual_matrix = initMat();
                    SDL_Delay(50);
                    draw(*actual_matrix);
                    restart = true;
                    break;
                  }
                }
                if(restart)
                {
                  break;
                }
              }
            }
          }
          else if(x >= 662 && x < 693)
          {
            if(!drawn)
            {
              loadText("Save Game", black, 640, 318, 120, 30);
              SDL_RenderPresent(gRenderer);
              drawn = true;
            }
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
              playSound(sBip[1]);
              saveGame(*actual_matrix, game_over);
              SDL_Delay(100);
              SDL_Rect fillRect = {630, 318, 140, 30};
              gTexture = loadTexture("Sprites/Hide_Text.png");
              SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
              SDL_DestroyTexture(gTexture);
              gTexture = NULL;
              SDL_RenderPresent(gRenderer);
              loadText("Game Saved", black, 640, 318, 120, 30);
              SDL_RenderPresent(gRenderer);
            }
          }
          else if(x >= 707 && x < 738)
          {
            if(!drawn)
            {
              loadText("Load Game", black, 640, 318, 120, 30);
              SDL_RenderPresent(gRenderer);
              drawn = true;
            }
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
              playSound(sBip[1]);
              if(loadGame(actual_matrix, game_over))
              {
                move = 0;
                SDL_Rect fillRect = {630, 318, 140, 30};
                gTexture = loadTexture("Sprites/Hide_Text.png");
                SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
                SDL_DestroyTexture(gTexture);
                gTexture = NULL;
                SDL_RenderPresent(gRenderer);
                draw(*actual_matrix);
                loadText("Game Loaded", black, 640, 318, 120, 30);
                gameOver(*actual_matrix, &game_over);
              }
            }
          }
          else if(x >= 752 && x < 783)
          {
            if(!drawn)
            {
              loadText("Main Menu", black, 640, 318, 120, 30);
              SDL_RenderPresent(gRenderer);
              drawn = true;
            }
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
              playSound(sBip[1]);
              SDL_Delay(125);
              mainMenu(actual_matrix, &game_over, &quit, &move);
            }
          }
          else if(drawn = true)
          {
            SDL_Rect fillRect = {630, 318, 140, 30};
            gTexture = loadTexture("Sprites/Hide_Text.png");
            SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
            SDL_DestroyTexture(gTexture);
            gTexture = NULL;
            SDL_RenderPresent(gRenderer);
            drawn = false;
          }
        }
        if((y < 360 || y >= 391) && drawn == true && event.type == SDL_MOUSEMOTION)
        {
          SDL_Delay(100);
          SDL_Rect fillRect = {630, 318, 140, 30};
          gTexture = loadTexture("Sprites/Hide_Text.png");
          SDL_RenderCopy(gRenderer, gTexture, NULL, &fillRect);
          SDL_DestroyTexture(gTexture);
          gTexture = NULL;
          SDL_RenderPresent(gRenderer);
          drawn = false;
        }
        if(x <= 580 && y <= 580 && event.type == SDL_MOUSEBUTTONDOWN && move % 2 == 0 && !game_over)
        {
          check_queen(*actual_matrix);
          check_pawn(*actual_matrix);
          mandatoryIndicator();
          if(!shown && (queen_mandatory || pawn_mandatory))
          {
            loadText("Obligatory move", black, 630, 240, 140, 30);
          }
          SDL_GetMouseState(&y, &x);
          getSize(y, x);
          x = mapToInt(x);
          y = mapToInt(y);
          if((!queen_mandatory && !pawn_mandatory) || (x == mandatory_x && y == mandatory_y))
          {
            playSound(sMove[0]);
          }
          if(indicator(y, x, *actual_matrix) || !indicator_on)
          {
            move++;
          }
        }
        else if(event.type == SDL_MOUSEBUTTONDOWN && move % 2 == 1)
        {
          if((!queen_mandatory && !pawn_mandatory) || (x == mandatory_x && y == mandatory_y))
          {
            playSound(sMove[1]);
          }
          SDL_GetMouseState(&v, &u);
          if(u <= 580 && v <= 580)
          {
            getSize(v, u);
            v = mapToInt(v);
            u = mapToInt(u);
            apply_move(x, y, u, v, actual_matrix);
            move++;
            shown = false;
            draw(*actual_matrix);
            gameOver(*actual_matrix, &game_over);
          }
        }
      }
    }
  }
  close();
}
