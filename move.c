#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "move.h"
#include "init_mat.h"
#include "gui.h"

int actual_player = 0;

typedef enum _bool
{
  false,
  true
} bool;

enum
{
  none,
  whites,
  blacks
} winner;

typedef enum _mstate
{
  impossible,
  normal,
  turn,
  destroy11,
  destroy12,
  destroy21,
  destroy22
} mstate;

int check_mat(int **actual_matrix)
{
  int player = none;
  int around_wk, around_bk, w_pieces, b_pieces;
  for(int x = 0; x < 15; x++)
  {
    for(int y = 0; y < 15; y++)
    {
      if(actual_matrix[x][y] == 23)
      {
        if(actual_matrix[x+1][y] != 0)
        {
          around_wk++;
          if(actual_matrix[x+1][y] / 100 == 1)
          {
            b_pieces++;
          }
        }
        if(actual_matrix[x-1][y] != 0)
        {
          around_wk++;
          if(actual_matrix[x-1][y] / 100 == 1)
          {
            b_pieces++;
          }
        }
        if(actual_matrix[x][y+1] != 0)
        {
          around_wk++;
          if(actual_matrix[x][y+1] / 100 == 1)
          {
            b_pieces++;
          }
        }
        if(actual_matrix[x][y-1] != 0)
        {
          around_wk++;
          if(actual_matrix[x][y-1] / 100 == 1)
          {
            b_pieces++;
          }
        }
      }
      else if(actual_matrix[x][y] == 13)
      {
        if(actual_matrix[x+1][y] != 0)
        {
          around_bk++;
          if(actual_matrix[x+1][y] / 100 == 2)
          {
            w_pieces++;
          }
        }
        if(actual_matrix[x-1][y] != 0)
        {
          around_bk++;
          if(actual_matrix[x-1][y] / 100 == 2)
          {
            w_pieces++;
          }
        }
        if(actual_matrix[x][y+1] != 0)
        {
          around_bk++;
          if(actual_matrix[x][y+1] / 100 == 2)
          {
            w_pieces++;
          }
        }
        if(actual_matrix[x][y-1] != 0)
        {
          around_bk++;
          if(actual_matrix[x][y-1] / 100 == 2)
          {
            w_pieces++;
          }
        }
      }
    }
  }
  if(around_bk == 4 && w_pieces >= 1)
  {
    player = whites;
  }
  else if(around_wk == 4 && b_pieces >= 1)
  {
    player = blacks;
  }
  return player;
}

int *check_queen(int **actual_matrix, int actual_player, int *p)
{
  if(actual_player % 2 == 0)
  {
    for(int x = 0; x < 15; x++)
    {
      for(int y = 0; y < 15; y++)
      {
        if(actual_matrix[x][y] == 221)
        {
          for(int z = x-2; z > 0; z -= 2)
          {
            if(actual_matrix[z][y] / 100 == 2 || actual_matrix[z+1][y] % 10 == 3)
            {
              break;
            }
            else if(actual_matrix[z][y] / 100 == 1)
            {
              if(actual_matrix[z-2][y] == 0)
              {
                p[0] = true;
                p[1] = x;
                p[2] = y;
                return p;
              }
            }
          }
          for(int z = x+2; z < 14; z += 2)
          {
            if(actual_matrix[z][y] / 100 == 2 || actual_matrix[z-1][y] % 10 == 3)
            {
              break;
            }
            else if(actual_matrix[z][y] / 100 == 1)
            {
              p[0] = true;
              p[1] = x;
              p[2] = y;
              return p;
            }
          }
        }
        else if(actual_matrix[x][y] == 220)
        {
          for(int z = y-2; z > 0; z -= 2)
          {
            if(actual_matrix[x][z] / 100 == 2 || actual_matrix[x][z+1] % 10 == 3)
            {
              break;
            }
            else if(actual_matrix[x][z] / 100 == 1)
            {
              if(actual_matrix[x][z-2] == 0)
              {
                p[0] = true;
                p[1] = x;
                p[2] = y;
                return p;
              }
            }
          }
          for(int z = y+2; z < 14; z += 2)
          {
            if(actual_matrix[x][z] / 100 == 2 || actual_matrix[x][z-1] % 10 == 3)
            {
              break;
            }
            else if(actual_matrix[x][z] / 100 == 1)
            {
              if(actual_matrix[x][z+2] == 0)
              {
                p[0] = true;
                p[1] = x;
                p[2] = y;
                return p;
              }
            }
          }
        }
      }
    }
  }
  else if(actual_player % 2 == 1)
  {
    for(int x = 0; x < 15; x++)
    {
      for(int y = 0; y < 15; y++)
      {
        if(actual_matrix[x][y] == 121)
        {
          for(int z = x-2; z > 0; z -= 2)
          {
            if(actual_matrix[z][y] / 100 == 1 || actual_matrix[z+1][y] % 10 == 3)
            {
              break;
            }
            else if(actual_matrix[z][y] / 100 == 2)
            {
              if(actual_matrix[z-2][y] == 0)
              {
                p[0] = true;
                p[1] = x;
                p[2] = y;
                return p;
              }
            }
          }
          for(int z = x+2; z < 14; z += 2)
          {
            if(actual_matrix[z][y] / 100 == 1 || actual_matrix[z-1][y] % 10 == 3)
            {
              break;
            }
            else if(actual_matrix[z][y] / 100 == 2)
            {
              p[0] = true;
              p[1] = x;
              p[2] = y;
              return p;
            }
          }
        }
        else if(actual_matrix[x][y] == 120)
        {
          for(int z = y-2; z > 0; z -= 2)
          {
            if(actual_matrix[x][z] / 100 == 1 || actual_matrix[x][z+1] % 10 == 3)
            {
              break;
            }
            else if(actual_matrix[x][z] / 100 == 2)
            {
              if(actual_matrix[x][z-2] == 0)
              {
                p[0] = true;
                p[1] = x;
                p[2] = y;
                return p;
              }
            }
          }
          for(int z = y+2; z < 14; z += 2)
          {
            if(actual_matrix[x][z] / 100 == 1 || actual_matrix[x][z-1] % 10 == 3)
            {
              break;
            }
            else if(actual_matrix[x][z] / 100 == 2)
            {
              if(actual_matrix[x][z+2] == 0)
              {
                p[0] = true;
                p[1] = x;
                p[2] = y;
                return p;
              }
            }
          }
        }
      }
    }
  }
  return p;
}

// Still need to code queens general movement && rotation...

mstate check_move(int x, int y, int u, int v, int **actual_matrix)
{
  if(actual_matrix[x][y] == 0 || actual_matrix[x][y] == -1 || actual_matrix[x][y] == -2)
  {
    printf("Source is not a playable piece...");
    return impossible;
  }
  else if((actual_matrix[x][y] / 100 == 1 || actual_matrix[x][y] / 10 == 1) && actual_player % 2 == 0)
  {
    printf("Not blacks turn...");
    return impossible;
  }
  else if((actual_matrix[x][y] / 100 == 2 || actual_matrix[x][y] / 10 == 2) && actual_player % 2 == 1)
  {
    printf("Not whites turn...");
    return impossible;
  }
  else if(actual_matrix[u][v] == -1 || (x == u && y == v) || (actual_matrix[x][y] / 100 == actual_matrix[u][v] / 100 && actual_matrix[x][y] % 10 != 3))
  {
    printf("Destination is not a playable tile...");
    return impossible;
  }
  else if(actual_matrix[x][y] != 13 && actual_matrix[x][y] != 23 && actual_matrix[u][v] == -2)
  {
    printf("Destination tile is for kings only...");
    return impossible;
  }
  else if(actual_player == 0)
  {
    if(actual_matrix[x][y] == 220)
    {
      if(u - x == 1 && abs(y - v) == 1)
      {
        actual_player++;
        printf("Possible move!");
        return turn;
      }
      printf("Impossible move!");
      return impossible;
    }
    else if(actual_matrix[x][y] == 210)
    {
      if(x - u == 1 && abs(y - v) == 1)
      {
        actual_player++;
        printf("Possible move!");
        return turn;
      }
      else if(x - u == 2 && y == v)
      {
        actual_player++;
        printf("Possible move!");
        return normal;
      }
      else
      {
        printf("Impossible move!");
        return impossible;
      }
    }
    else if(actual_matrix[x][y] == 211)
    {
      if((x - u == 2 || x - u == 4) && y == v)
      {
        actual_player++;
        printf("Possible move!");
        return normal;
      }
      else
      {
        printf("Impossible move!");
        return impossible;
      }
    }
    else
    {
      printf("Impossible move!");
      return impossible;
    }
  }
  else if(actual_player == 1)
  {
    if(actual_matrix[x][y] == 120)
    {
      if(x - u == 1 && abs(y - v) == 1)
      {
        actual_player++;
        printf("Possible move!");
        return turn;
      }
      else
      {
        printf("Impossible move!");
        return impossible;
      }
    }
    else if(actual_matrix[x][y] == 110)
    {
      if(u - x == 1 && abs(y - v) == 1)
      {
        actual_player++;
        printf("Possible move!");
        return turn;
      }
      else if(u - x == 2 && y == v)
      {
        actual_player++;
        printf("Possible move!");
        return normal;
      }
      else
      {
        printf("Impossible move!");
        return impossible;
      }
    }
    else if(actual_matrix[x][y] == 111)
    {
      if((u - x == 2 || u - x == 4) && y == v)
      {
        actual_player++;
        printf("Possible move!");
        return normal;
      }
      else
      {
        printf("Impossible move!");
        return impossible;
      }
    }
    else
    {
      printf("Impossible move!");
      return impossible;
    }
  }
  else if(actual_matrix[x][y] == 23)
  {
    if(actual_matrix[u][v] != -2)
    {
      printf("Impossible move!");
      return impossible;
    }
    else if(((u+2 < 15 && actual_matrix[u+2][v] != 13) || u+2 >= 15) && ((v+2 < 15 && actual_matrix[u][v+2] != 13) || v+2 >= 15) && ((u+2 < 15 && v+2 < 15 && actual_matrix[u+2][v+2] != 13) || u+2 >= 15 || v+2 >= 15) && ((u-2 >= 0 && actual_matrix[u-2][v] != 13) || u-2 < 0) && ((v-2 >= 0 && actual_matrix[u][v-2] != 13) || v-2 < 0) && ((u-2 >= 0 && v-2 >= 0 && actual_matrix[u-2][v-2] != 13) || u-2 < 0 || v - 2 < 0) && ((u+2 < 15 && v-2 >= 0 && actual_matrix[u+2][v-2] != 13) || u+2 >= 15 || v-2 < 0) && ((u-2 >= 0 && v+2 < 15 && actual_matrix[u-2][v+2] != 13) || u-2 < 0 || v+2 >= 15))
    {
      if(x == u)
      {
        if((v - y == 2 && actual_matrix[x][y + 1] == 0) || (y - v == 2 && actual_matrix[x][y - 1] == 0))
        {
          actual_player++;
          printf("Possible move!");
          return normal;
        }
        printf("Impossible move!");
        return impossible;
      }
      else if(y == v)
      {
        if((x - u == 2 && actual_matrix[x - 1][y] == 0) || (u - x == 2 && actual_matrix[x + 1][y] == 0))
        {
          actual_player++;
          printf("Possible move!");
          return normal;
        }
        printf("Impossible move!");
        return impossible;
      }
      printf("Impossible move!");
      return impossible;
    }
    printf("Impossible move!");
    return impossible;
  }
  else if(actual_matrix[x][y] == 13)
  {
    if(actual_matrix[u][v] != -2)
    {
      printf("Impossible move!");
      return impossible;
    }
    else if(((u+2 < 15 && actual_matrix[u+2][v] != 23) || u+2 >= 15) && ((v+2 < 15 && actual_matrix[u][v+2] != 23) || v+2 >= 15) && ((u+2 < 15 && v+2 < 15 && actual_matrix[u+2][v+2] != 23) || u+2 >= 15 || v+2 >= 15) && ((u-2 >= 0 && actual_matrix[u-2][v] != 23) || u-2 < 0) && ((v-2 >= 0 && actual_matrix[u][v-2] != 23) || v-2 < 0) && ((u-2 >= 0 && v-2 >= 0 && actual_matrix[u-2][v-2] != 23) || u-2 < 0 || v - 2 < 0) && ((u+2 < 15 && v-2 >= 0 && actual_matrix[u+2][v-2] != 23) || u+2 >= 15 || v-2 < 0) && ((u-2 >= 0 && v+2 < 15 && actual_matrix[u-2][v+2] != 23) || u-2 < 0 || v+2 >= 15))
    {
      if(x == u)
      {
        if((v - y == 2 && actual_matrix[x][y + 1] == 0) || (y - v == 2 && actual_matrix[x][y - 1] == 0))
        {
          actual_player++;
          printf("Possible move!");
          return normal;
        }
        printf("Impossible move!");
        return impossible;
      }
      else if(y == v)
      {
        if((x - u == 2 && actual_matrix[x - 1][y] == 0) || (u - x == 2 && actual_matrix[x + 1][y] == 0))
        {
          actual_player++;
          printf("Possible move!");
          return normal;
        }
        printf("Impossible move!");
        return impossible;
      }
      printf("Impossible move!");
      return impossible;
    }
    printf("Impossible move!");
    return impossible;
  }
  else if(actual_matrix[x][y] == 110 || actual_matrix[x][y] == 111)
  {
    if(y == v)
    {
      if(y % 2 == 1 && actual_matrix[u][v] == 0)
      {
        if(u - x == 2 && actual_matrix[x+1][v] % 10 != 3)
        {
          actual_player++;
          printf("Possible move!");
          return normal;
        }
        else if(x + 2 < 15 && actual_matrix[x+2][y] / 100 == 2 && u - x > 2 && actual_matrix[x+1][y] % 10 != 3)
        {
          if(x + 4 < 15 && actual_matrix[x+4][y] == 0 && actual_matrix[x+3][y] % 10 != 3)
          {
            if(((x + 6 < 15 && actual_matrix[x+6][y] / 100 != 2) || (x + 6 >= 15) || (actual_matrix[x+5][y] % 10 == 3)) && u - x == 4)
            {
              actual_player++;
              printf("Possible move!");
              return destroy11;
            }
            else if(x + 6 < 15 && actual_matrix[x+6][y] / 100 == 2 && actual_matrix[x+5][y] % 10 != 3)
            {
              if(x + 8 < 15 && actual_matrix[x+8][y] == 0 && actual_matrix[x+7][y] % 10 != 3)
              {
                if(((x + 10 < 15 && actual_matrix[x+10][y] / 100 != 2) || (x + 10 >= 15) || (actual_matrix[x+9][y] % 10 == 3)) && u - x == 8)
                {
                  actual_player++;
                  printf("Possible move!");
                  return destroy12;
                }
                printf("Impossible move!");
                return impossible;
              }
              printf("Impossible move!");
              return impossible;
            }
            printf("Impossible move!");
            return impossible;
          }
          printf("Impossible move!");
          return impossible;
        }
        printf("Impossible move!");
        return impossible;
      }
      else if(y % 2 == 0)
      {
        if(u - x == 2 && actual_matrix[u][v] == 0)
        {
          actual_player++;
          printf("Possible move!");
          return normal;
        }
        else if(x + 2 < 15 && actual_matrix[x+2][y] / 100 == 2 && u - x == 2)
        {
          actual_player++;
          printf("Possible move!");
          return normal;
        }
        printf("Impossible move!");
        return impossible;
      }
    }
    else if(abs(y - v) == 1 && u - x == 1)
    {
      if(actual_matrix[u][v] == 0)
      {
        actual_player++;
        printf("Possible move!");
        return turn;
      }
      printf("Impossible move!");
      return impossible;
    }
    printf("Impossible move!");
    return impossible;
  }
  else if(actual_matrix[x][y] == 210 || actual_matrix[x][y] == 211)
  {
    if(y == v)
    {
      if(y % 2 == 1 && actual_matrix[u][v] == 0)
      {
        if(x - u == 2 && actual_matrix[x-1][v] % 10 != 3)
        {
          actual_player++;
          printf("Possible move!");
          return normal;
        }
        else if(x - 2 >= 0 && actual_matrix[x-2][y] / 100 == 1 && x - u > 2 && actual_matrix[x-1][y] % 10 != 3)
        {
          if(x - 4 >= 0 && actual_matrix[x-4][y] == 0 && actual_matrix[x-3][y] % 10 != 3)
          {
            if(((x - 6 >= 0 && actual_matrix[x-6][y] / 100 != 1) || (x - 6 < 0) || (actual_matrix[x-5][y] % 10 == 3)) && x - u == 4)
            {
              actual_player++;
              printf("Possible move!");
              return destroy21;
            }
            else if(x - 6 >= 0 && actual_matrix[x-6][y] / 100 == 1 && actual_matrix[x-5][y] % 10 != 3)
            {
              if(x - 8 >= 0 && actual_matrix[x-8][y] == 0 && actual_matrix[x-7][y] % 10 != 3)
              {
                if(((x - 10 >= 0 && actual_matrix[x-10][y] / 100 != 1) || (x - 10 < 0) || (actual_matrix[x-9][y] % 10 == 3)) && x - u == 8)
                {
                  actual_player++;
                  printf("Possible move!");
                  return destroy22;
                }
                printf("Impossible move!");
                return impossible;
              }
              printf("Impossible move!");
              return impossible;
            }
            printf("Impossible move!");
            return impossible;
          }
          printf("Impossible move!");
          return impossible;
        }
        printf("Impossible move!");
        return impossible;
      }
      else if(y % 2 == 0)
      {
        if(x - u == 2 && actual_matrix[u][v] == 0)
        {
          actual_player++;
          printf("Possible move!");
          return normal;
        }
        if(x - 2 >= 0 && actual_matrix[x-2][y] / 100 == 1 && x - u == 2)
        {
          actual_player++;
          printf("Possible move!");
          return normal;
        }
        printf("Impossible move!");
        return impossible;
      }
    }
    else if(abs(y - v) == 1 && x - u == 1)
    {
      if(actual_matrix[u][v] == 0)
      {
        actual_player++;
        printf("Possible move!");
        return turn;
      }
      printf("Impossible move!");
      return impossible;
    }
    printf("Impossible move!");
    return impossible;
  }
  /*else if(actual_matrix[x][y] == 120 || actual_matrix[x][y] == 121)
  {
    if(actual_matrix[x][y] == 120 && actual_matrix[u][v] == 0)
    {
      if(abs(x - u) == 1 && abs(y - v) == 1)
      {
        actual_player++;
        printf("Possible move!");
        return turn;
      }
      else if(y == v)
      {
        if(u - x == 2 && actual_matrix[u][v] == 0)
        {

        }
      }
      printf("Impossible move!");
      return impossible;
    }
    printf("Impossible move!");
    return impossible;
  }*/
  printf("Not yet implemented!");
  return impossible;
}

void apply_move(int x, int y, int u, int v, int ***actual_matrix)
{
  int temp = (*actual_matrix)[x][y];
  mstate apply = check_move(x, y, u, v, *actual_matrix);
  if(apply == normal || apply == turn)
  {
    (*actual_matrix)[u][v] = (*actual_matrix)[x][y];
    if(apply == turn)
    {
      if((*actual_matrix)[u][v] % 2 == 0)
      {
        (*actual_matrix)[u][v]++;
      }
      else
      {
        (*actual_matrix)[u][v]--;
      }
    }
    if((*actual_matrix)[x][y] % 10 == 3)
    {
      (*actual_matrix)[x][y] = -2;
    }
    else
    {
      (*actual_matrix)[x][y] = 0;
    }
  }
  else if(apply == destroy11 || apply == destroy12)
  {
    (*actual_matrix)[x+2][y] = 0;
    (*actual_matrix)[x+4][y] = (*actual_matrix)[x][y];
    (*actual_matrix)[x][y] = 0;
    if(apply == destroy12)
    {
      draw(*actual_matrix);
      SDL_Delay(200);
      (*actual_matrix)[x+4][y] = 0;
      SDL_Delay(200);
      (*actual_matrix)[x+6][y] = 0;
      (*actual_matrix)[x+8][y] = temp;
    }
  }
  else if(apply == destroy21 || apply == destroy22)
  {
    (*actual_matrix)[x-2][y] = 0;
    (*actual_matrix)[x-4][y] = (*actual_matrix)[x][y];
    (*actual_matrix)[x][y] = 0;
    if(apply == destroy22)
    {
      draw(*actual_matrix);
      SDL_Delay(200);
      (*actual_matrix)[x-4][y] = 0;
      SDL_Delay(200);
      (*actual_matrix)[x-6][y] = 0;
      (*actual_matrix)[x-8][y] = temp;
    }
  }
  if(apply != impossible)
  {
    if(temp / 100 == 1 && u == 14)
    {
      draw(*actual_matrix);
      SDL_Delay(600);
      (*actual_matrix)[u][v] = 121;
    }
    else if(temp / 100 == 2 && u == 0)
    {
      draw(*actual_matrix);
      SDL_Delay(600);
      (*actual_matrix)[u][v] = 221;
    }
  }
}
