#include <stdio.h>
#include <stdlib.h>
#include "init_mat.h"
#include "gui.h"

int actual_player = 0;
bool pawn_mandatory = false, queen_mandatory = false;
int mandatory_x, mandatory_y;

winner check_mat(int **actual_matrix)
{
  winner player = none;
  int around_wk = 0, around_bk = 0, w_pieces = 0, b_pieces = 0;
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

bool check_null(int **actual_matrix)
{
  int w_number = 0, b_number = 0;
  for(int i = 0; i < 15; i++)
  {
    for(int j = 0; j < 15; j++)
    {
      if(actual_matrix[i][j] / 100 == 2)
      {
        w_number++;
      }
      else if(actual_matrix[i][j] / 100 == 1)
      {
        b_number++;
      }
    }
  }
  for(int i = 0; i < 15; i++)
  {
    for(int j = 0; j < 15; j++)
    {
      if(((actual_matrix[i][j] == 23 && w_number == 0) || (actual_matrix[i][j] == 13 && b_number == 0)) && !check_move(i, j, i-1, j, actual_matrix) && !check_move(i, j, i+1, j, actual_matrix) && !check_move(i, j, i, j+1, actual_matrix) && !check_move(i, j, i, j-1, actual_matrix))
      {
        return true;
      }
      else if(w_number < 4 && b_number < 4)
      {
        return true;
      }
    }
  }
  return false;
}

void check_pawn(int **actual_matrix)
{
  bool found = false;
  if(actual_player % 2 == 0)
  {
    for(int x = 0; x < 15; x++)
    {
      for(int y = 0; y < 15; y++)
      {
        if(actual_matrix[x][y] == 211 && x-4 >= 0 && actual_matrix[x-1][y] == -2 && actual_matrix[x-2][y] / 100 == 1 && actual_matrix[x-3][y] == -2 && actual_matrix[x-4][y] == 0 && !queen_mandatory)
        {
          pawn_mandatory = true;
          found = true;
          mandatory_x = x;
          mandatory_y = y;
          playSound(sMandatory);
          mandatoryIndicator();
          if(!shown)
          {
            loadText("Obligatory move", black, 630, 240, 140, 30);
          }
          break;
        }
      }
      if(found)
      {
        break;
      }
    }
  }
  else
  {
    for(int x = 0; x < 15; x++)
    {
      for(int y = 0; y < 15; y++)
      {
        if(actual_matrix[x][y] == 111 && x+4 < 15 && actual_matrix[x+1][y] == -2 && actual_matrix[x+2][y] / 100 == 2 && actual_matrix[x+3][y] == -2 && actual_matrix[x+4][y] == 0 && !queen_mandatory)
        {
          pawn_mandatory = true;
          found = true;
          mandatory_x = x;
          mandatory_y = y;
          playSound(sMandatory);
          mandatoryIndicator();
          if(!shown)
          {
            loadText("Obligatory move", black, 630, 240, 140, 30);
          }
          break;
        }
      }
      if(found)
      {
        break;
      }
    }
  }
  SDL_DestroyTexture(gTexture);
  gTexture = NULL;
  SDL_RenderPresent(gRenderer);
  shown = true;
}

void check_queen(int **actual_matrix)
{
  bool found = false, perma_found = false;
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
            if(actual_matrix[z][y] / 100 == 2 || actual_matrix[z+1][y] % 10 == 3 || (actual_matrix[z][y] / 100 == 1 && z-2 > 0 && actual_matrix[z-2][y] / 100 == 1))
            {
              break;
            }
            else if(actual_matrix[z][y] / 100 == 1)
            {
              if(actual_matrix[z-2][y] == 0 && actual_matrix[z-1][y] == -2 && (z + 2 == x || actual_matrix[z+2][y] == 0))
              {
                queen_mandatory = true;
                perma_found = found = true;
                mandatory_x = x;
                mandatory_y = y;
                mandatoryIndicator();
              }
            }
            if(found)
            {
              found = false;
              break;
            }
          }
          for(int z = x+2; z < 14; z += 2)
          {
            if(actual_matrix[z][y] / 100 == 2 || actual_matrix[z-1][y] % 10 == 3 || (actual_matrix[z][y] / 100 == 1 && z+2 < 14 && actual_matrix[z+2][y] / 100 == 1))
            {
              break;
            }
            else if(actual_matrix[z][y] / 100 == 1)
            {
              if(actual_matrix[z+2][y] == 0 && actual_matrix[z+1][y] == -2 && (z - 2 == x || actual_matrix[z-2][y] == 0))
              {
                queen_mandatory = true;
                perma_found = found = true;
                mandatory_x = x;
                mandatory_y = y;
                mandatoryIndicator();
              }
            }
            if(found)
            {
              found = false;
              break;
            }
          }
        }
        else if(actual_matrix[x][y] == 220)
        {
          for(int z = y-2; z > 0; z -= 2)
          {
            if(actual_matrix[x][z] / 100 == 2 || actual_matrix[x][z+1] % 10 == 3 || (actual_matrix[x][z] / 100 == 1 && z-2 > 0 && actual_matrix[x][z-2] / 100 == 1))
            {
              break;
            }
            else if(actual_matrix[x][z] / 100 == 1)
            {
              if(actual_matrix[x][z-2] == 0 && actual_matrix[x][z-1] == -2 && (z + 2 == y || actual_matrix[x][z+2] == 0))
              {
                queen_mandatory = true;
                perma_found = found = true;
                mandatory_x = x;
                mandatory_y = y;
                mandatoryIndicator();
              }
            }
            if(found)
            {
              found = false;
              break;
            }
          }
          for(int z = y+2; z < 14; z += 2)
          {
            if(actual_matrix[x][z] / 100 == 2 || actual_matrix[x][z-1] % 10 == 3 || (actual_matrix[x][z] / 100 == 1 && z+2 < 14 && actual_matrix[x][z+2] / 100 == 1))
            {
              break;
            }
            else if(actual_matrix[x][z] / 100 == 1)
            {
              if(actual_matrix[x][z+2] == 0 && actual_matrix[x][z+1] == -2 && (z - 2 == y || actual_matrix[x][z-2] == 0))
              {
                queen_mandatory = true;
                perma_found = found = true;
                mandatory_x = x;
                mandatory_y = y;
                mandatoryIndicator();
              }
            }
            if(found)
            {
              found = false;
              break;
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
            if(actual_matrix[z][y] / 100 == 1 || actual_matrix[z+1][y] % 10 == 3 || (actual_matrix[z][y] / 100 == 2 && z-2 > 0 && actual_matrix[z-2][y] / 100 == 2))
            {
              break;
            }
            else if(actual_matrix[z][y] / 100 == 2)
            {
              if(actual_matrix[z-2][y] == 0 && actual_matrix[z-1][y] == -2 && (z+2 == x || actual_matrix[z+2][y] == 0))
              {
                queen_mandatory = true;
                perma_found = found = true;
                mandatory_x = x;
                mandatory_y = y;
                mandatoryIndicator();
              }
            }
            if(found)
            {
              found = false;
              break;
            }
          }
          for(int z = x+2; z < 14; z += 2)
          {
            if(actual_matrix[z][y] / 100 == 1 || actual_matrix[z-1][y] % 10 == 3 || (actual_matrix[z][y] / 100 == 2 && z+2 < 14 && actual_matrix[z+2][y] / 100 == 2))
            {
              break;
            }
            else if(actual_matrix[z][y] / 100 == 2)
            {
              if(actual_matrix[z+2][y] == 0 && actual_matrix[z+1][y] == -2 && (z - 2 == x || actual_matrix[z-2][y] == 0))
              {
                queen_mandatory = true;
                perma_found = found = true;
                mandatory_x = x;
                mandatory_y = y;
                mandatoryIndicator();
              }
            }
            if(found)
            {
              found = false;
              break;
            }
          }
        }
        else if(actual_matrix[x][y] == 120)
        {
          for(int z = y-2; z > 0; z -= 2)
          {
            if(actual_matrix[x][z] / 100 == 1 || actual_matrix[x][z+1] % 10 == 3 || (actual_matrix[x][z] / 100 == 2 && z-2 > 0 && actual_matrix[x][z-2] / 100 == 2))
            {
              break;
            }
            else if(actual_matrix[x][z] / 100 == 2)
            {
              if(actual_matrix[x][z-2] == 0 && actual_matrix[x][z-1] == -2 && (z + 2 == y || actual_matrix[x][z+2] == 0))
              {
                queen_mandatory = true;
                perma_found = found = true;
                mandatory_x = x;
                mandatory_y = y;
                mandatoryIndicator();
              }
            }
            if(found)
            {
              found = false;
              break;
            }
          }
          for(int z = y+2; z < 14; z += 2)
          {
            if(actual_matrix[x][z] / 100 == 1 || actual_matrix[x][z-1] % 10 == 3 || (actual_matrix[x][z] / 100 == 2 && z+2 < 14 && actual_matrix[x][z+2] / 100 == 2))
            {
              break;
            }
            else if(actual_matrix[x][z] / 100 == 2)
            {
              if(actual_matrix[x][z+2] == 0 && actual_matrix[x][z+1] == -2 && (z - 2 == y || actual_matrix[x][z-2] == 0))
              {
                queen_mandatory = true;
                perma_found = found = true;
                mandatory_x = x;
                mandatory_y = y;
                mandatoryIndicator();
              }
            }
            if(found)
            {
              found = false;
              break;
            }
          }
        }
      }
    }
  }
  if(perma_found)
  {
    SDL_DestroyTexture(gTexture);
    gTexture = NULL;
    SDL_RenderPresent(gRenderer);
    shown = true;
    if(!shown)
    {
      loadText("Obligatory move", black, 630, 240, 140, 30);
    }
    playSound(sMandatory);
  }
}

mstate check_move(int x, int y, int u, int v, int **actual_matrix)
{
  if(actual_matrix[x][y] == 0 || actual_matrix[x][y] == -1 || actual_matrix[x][y] == -2)
  {
    return impossible;
  }
  else if((actual_matrix[x][y] / 100 == 1 || actual_matrix[x][y] / 10 == 1) && actual_player % 2 == 0)
  {
    return impossible;
  }
  else if((actual_matrix[x][y] / 100 == 2 || actual_matrix[x][y] / 10 == 2) && actual_player % 2 == 1)
  {
    return impossible;
  }
  else if(actual_matrix[u][v] == -1 || (x == u && y == v) || (actual_matrix[x][y] / 100 == actual_matrix[u][v] / 100 && actual_matrix[x][y] % 10 != 3))
  {
    return impossible;
  }
  else if(actual_matrix[x][y] != 13 && actual_matrix[x][y] != 23 && actual_matrix[u][v] == -2)
  {
    return impossible;
  }
  /*else if(actual_player == 0)
  {
    if(actual_matrix[x][y] == 220)
    {
      if(u - x == 1 && abs(y - v) == 1)
      {
        return turn;
      }
      return impossible;
    }
    else if(actual_matrix[x][y] == 210)
    {
      if(x - u == 1 && abs(y - v) == 1)
      {
        return turn;
      }
      else if(x - u == 2 && y == v)
      {
        return normal;
      }
      else
      {
        return impossible;
      }
    }
    else if(actual_matrix[x][y] == 211)
    {
      if((x - u == 2 || x - u == 4) && y == v)
      {
        return normal;
      }
      else
      {
        return impossible;
      }
    }
    else
    {
      return impossible;
    }
  }
  else if(actual_player == 1)
  {
    if(actual_matrix[x][y] == 120)
    {
      if(x - u == 1 && abs(y - v) == 1)
      {
        return turn;
      }
      else
      {
        return impossible;
      }
    }
    else if(actual_matrix[x][y] == 110)
    {
      if(u - x == 1 && abs(y - v) == 1)
      {
        return turn;
      }
      else if(u - x == 2 && y == v)
      {
        return normal;
      }
      else
      {
        return impossible;
      }
    }
    else if(actual_matrix[x][y] == 111)
    {
      if((u - x == 2 || u - x == 4) && y == v)
      {
        return normal;
      }
      else
      {
        return impossible;
      }
    }
    else
    {
      return impossible;
    }
  }*/
  else if(actual_matrix[x][y] == 23 && !pawn_mandatory && !queen_mandatory)
  {
    if(actual_matrix[u][v] != -2)
    {
      return impossible;
    }
    else if(((u+2 < 15 && actual_matrix[u+2][v] != 13) || u+2 >= 15) && ((v+2 < 15 && actual_matrix[u][v+2] != 13) || v+2 >= 15) && ((u+2 < 15 && v+2 < 15 && actual_matrix[u+2][v+2] != 13) || u+2 >= 15 || v+2 >= 15) && ((u-2 >= 0 && actual_matrix[u-2][v] != 13) || u-2 < 0) && ((v-2 >= 0 && actual_matrix[u][v-2] != 13) || v-2 < 0) && ((u-2 >= 0 && v-2 >= 0 && actual_matrix[u-2][v-2] != 13) || u-2 < 0 || v - 2 < 0) && ((u+2 < 15 && v-2 >= 0 && actual_matrix[u+2][v-2] != 13) || u+2 >= 15 || v-2 < 0) && ((u-2 >= 0 && v+2 < 15 && actual_matrix[u-2][v+2] != 13) || u-2 < 0 || v+2 >= 15))
    {
      if(x == u)
      {
        if((v - y == 2 && actual_matrix[x][y + 1] == 0) || (y - v == 2 && actual_matrix[x][y - 1] == 0))
        {
          return normal;
        }
        return impossible;
      }
      else if(y == v)
      {
        if((x - u == 2 && actual_matrix[x - 1][y] == 0) || (u - x == 2 && actual_matrix[x + 1][y] == 0))
        {
          return normal;
        }
        return impossible;
      }
      return impossible;
    }
    return impossible;
  }
  else if(actual_matrix[x][y] == 13 && !queen_mandatory && !pawn_mandatory)
  {
    if(actual_matrix[u][v] != -2)
    {
      return impossible;
    }
    else if(((u+2 < 15 && actual_matrix[u+2][v] != 23) || u+2 >= 15) && ((v+2 < 15 && actual_matrix[u][v+2] != 23) || v+2 >= 15) && ((u+2 < 15 && v+2 < 15 && actual_matrix[u+2][v+2] != 23) || u+2 >= 15 || v+2 >= 15) && ((u-2 >= 0 && actual_matrix[u-2][v] != 23) || u-2 < 0) && ((v-2 >= 0 && actual_matrix[u][v-2] != 23) || v-2 < 0) && ((u-2 >= 0 && v-2 >= 0 && actual_matrix[u-2][v-2] != 23) || u-2 < 0 || v - 2 < 0) && ((u+2 < 15 && v-2 >= 0 && actual_matrix[u+2][v-2] != 23) || u+2 >= 15 || v-2 < 0) && ((u-2 >= 0 && v+2 < 15 && actual_matrix[u-2][v+2] != 23) || u-2 < 0 || v+2 >= 15))
    {
      if(x == u)
      {
        if((v - y == 2 && actual_matrix[x][y + 1] == 0) || (y - v == 2 && actual_matrix[x][y - 1] == 0))
        {
          return normal;
        }
        return impossible;
      }
      else if(y == v)
      {
        if((x - u == 2 && actual_matrix[x - 1][y] == 0) || (u - x == 2 && actual_matrix[x + 1][y] == 0))
        {
          return normal;
        }
        return impossible;
      }
      return impossible;
    }
    return impossible;
  }
  else if((actual_matrix[x][y] == 110 || actual_matrix[x][y] == 111) && !queen_mandatory)
  {
    if(y == v)
    {
      if(actual_matrix[x][y] == 111 && x == 2 && u - x == 4 && actual_matrix[x+1][y] == -2 && actual_matrix[x+2][y] / 100 != 1 && actual_matrix[x+3][y] == -2 && actual_matrix[x+4][y] == 0)
      {
        if((y == 1 || y == 3 || y == 5 || y == 7 || y == 9 || y == 11 || y == 13))
        {
          if(actual_matrix[x+2][y] == 0 && !pawn_mandatory)
          {
            return normal;
          }
          else if(actual_matrix[x+2][y] / 100 == 2 && (x+6 >= 15 || actual_matrix[x+6][y] / 100 != 2))
          {
            return destroy110;
          }
        }
      }
      if(y % 2 == 1 && actual_matrix[u][v] == 0)
      {
        if(u - x == 2 && actual_matrix[x+1][v] % 10 != 3 && !pawn_mandatory)
        {
          return normal;
        }
        else if(x + 2 < 15 && actual_matrix[x+2][y] / 100 == 2 && u - x > 2 && actual_matrix[x+1][y] % 10 != 3)
        {
          if(x + 4 < 15 && actual_matrix[x+4][y] == 0 && actual_matrix[x+3][y] % 10 != 3)
          {
            if(((x + 6 < 14 && actual_matrix[x+6][y] / 100 != 2) || (x + 6 >= 14) || (actual_matrix[x+5][y] % 10 == 3)) && u - x == 4)
            {
              return destroy110;
            }
            else if(x + 6 < 15 && actual_matrix[x+6][y] / 100 == 2 && actual_matrix[x+5][y] % 10 != 3)
            {
              if(x + 8 < 15 && (actual_matrix[x+7][y] % 10 == 3 || actual_matrix[x+8][y] != 0) && u - x == 4)
              {
                return destroy110;
              }
              else if(x + 8 < 15 && actual_matrix[x+8][y] == 0 && actual_matrix[x+7][y] % 10 != 3)
              {
                if(((x + 10 < 14 && actual_matrix[x+10][y] / 100 != 2) || (x + 10 >= 14) || (actual_matrix[x+9][y] % 10 == 3) || (actual_matrix[x+10][y] / 100 == 2 && ((x + 11 < 14 && actual_matrix[x+11][y] != -2) || (x + 12 < 15 && actual_matrix[x+12][y] / 100 == 2)))) && u - x == 8)
                {
                  return destroy111;
                }
                else if(u - x == 12 && actual_matrix[x+10][y] / 100 == 2 && actual_matrix[x+9][y] == -2 && actual_matrix[x+11][y] == -2)
                {
                  return destroy_max11;
                }
                return impossible;
              }
              return impossible;
            }
            return impossible;
          }
          return impossible;
        }
        return impossible;
      }
      else if(y % 2 == 0)
      {
        if(u - x == 2 && actual_matrix[u][v] == 0 && !pawn_mandatory)
        {
          return normal;
        }
        /*else if(x + 2 < 15 && actual_matrix[x+2][y] / 100 == 2 && u - x == 2 && !pawn_mandatory)
        {
          return normal;
        }*/
        return impossible;
      }
    }
    else if(abs(y - v) == 1 && u - x == 1 && !pawn_mandatory)
    {
      if(actual_matrix[u][v] == 0)
      {
        return turn;
      }
      return impossible;
    }
    return impossible;
  }
  else if((actual_matrix[x][y] == 210 || actual_matrix[x][y] == 211) && !queen_mandatory)
  {
    if(y == v)
    {
      if(actual_matrix[x][y] == 211 && x == 12 && x - u == 4 && actual_matrix[x-1][y] == -2 && actual_matrix[x-2][y] / 100 != 2 && actual_matrix[x-3][y] == -2 && actual_matrix[x-4][y] == 0)
      {
        if(y == 1 || y == 3 || y == 5 || y == 7 || y == 9 || y == 11 || y == 13)
        {
          if(actual_matrix[x-2][y] == 0 && !pawn_mandatory)
          {
            return normal;
          }
          else if(actual_matrix[x-2][y] / 100 == 1  && (x-6 < 0 || actual_matrix[x-6][y] / 100 != 1))
          {
            return destroy210;
          }
        }
      }
      if(y % 2 == 1 && actual_matrix[u][v] == 0)
      {
        if(x - u == 2 && actual_matrix[x-1][v] % 10 != 3 && !pawn_mandatory)
        {
          return normal;
        }
        else if(x - 2 >= 0 && actual_matrix[x-2][y] / 100 == 1 && x - u > 2 && actual_matrix[x-1][y] % 10 != 3)
        {
          if(x - 4 >= 0 && actual_matrix[x-4][y] == 0 && actual_matrix[x-3][y] % 10 != 3)
          {
            if(((x - 6 > 0 && actual_matrix[x-6][y] / 100 != 1) || (x - 6 <= 0) || (actual_matrix[x-5][y] % 10 == 3)) && x - u == 4)
            {
              return destroy210;
            }
            else if(x - 6 >= 0 && actual_matrix[x-6][y] / 100 == 1 && actual_matrix[x-5][y] % 10 != 3)
            {
              if(x - 8 >= 0 && (actual_matrix[x-7][y] % 10 == 3 || actual_matrix[x-8][y] != 0) && x - u == 4)
              {
                return destroy210;
              }
              else if(x - 8 >= 0 && actual_matrix[x-8][y] == 0 && actual_matrix[x-7][y] % 10 != 3)
              {
                if(((x - 10 > 0 && actual_matrix[x-10][y] / 100 != 1) || (x - 10 <= 0) || (actual_matrix[x-9][y] % 10 == 3) || (actual_matrix[x-10][y] / 100 == 1 && ((x - 11 > 0 && actual_matrix[x-11][y] != -2) || (x - 12 >= 0 && actual_matrix[x-12][y] / 100 == 1)))) && x - u == 8)
                {
                  return destroy211;
                }
                else if(x - u == 12 && actual_matrix[x-10][y] / 100 == 1 && actual_matrix[x-9][y] == -2 && actual_matrix[x-11][y] == -2)
                {
                  return destroy_max21;
                }
                return impossible;
              }
              return impossible;
            }
            return impossible;
          }
          return impossible;
        }
        return impossible;
      }
      else if(y % 2 == 0)
      {
        if(x - u == 2 && actual_matrix[u][v] == 0 && !pawn_mandatory)
        {
          return normal;
        }
        /*if(x-2 >= 0 && actual_matrix[x-2][y] / 100 == 1 && x - u == 2 && !pawn_mandatory)
        {
          return normal;
        }*/
        return impossible;
      }
    }
    else if(abs(y - v) == 1 && x - u == 1 && !pawn_mandatory)
    {
      if(actual_matrix[u][v] == 0)
      {
        return turn;
      }
      return impossible;
    }
    return impossible;
  }
  else if(actual_matrix[x][y] == 120 || actual_matrix[x][y] == 121)
  {
    if(actual_matrix[x][y] == 120)
    {
      if(abs(x - u) == 1 && abs(y - v) == 1 && actual_matrix[u][v] == 0 && !queen_mandatory && !pawn_mandatory)
      {
        return turn;
      }
      else if(y == v)
      {
        if(abs(u - x) == 2 && (/*actual_matrix[u][v] / 100 == 2 ||*/ actual_matrix[u][v] == 0) && !queen_mandatory && !pawn_mandatory)
        {
          if(u - x == 2 && actual_matrix[x+1][y] % 10 != 3)
          {
            return normal;
          }
          else if(x - u == 2 && actual_matrix[x-1][y] % 10 != 3)
          {
            return normal;
          }
          return impossible;
        }
        else if(u - x > 2 && actual_matrix[u][v] == 0 && !queen_mandatory && !pawn_mandatory)
        {
          for(int z = x+2; z <= u; z += 2)
          {
            if(actual_matrix[z-1][y] % 10 == 3 || actual_matrix[z][y] != 0)
            {
              return impossible;
            }
          }
          return normal;
        }
        else if(x - u > 2 && actual_matrix[u][v] == 0 && !queen_mandatory && !pawn_mandatory)
        {
          for(int z = x-2; z >= u; z -= 2)
          {
            if(actual_matrix[z+1][y] % 10 == 3 || actual_matrix[z][y] != 0)
            {
              return impossible;
            }
          }
          return normal;
        }
        return impossible;
      }
      else if(x == u && actual_matrix[u][v] == 0)
      {
        if(v > y)
        {
          for(int z = v+2; z < 14; z += 2)
          {
            if(actual_matrix[x][z-1] % 10 != 3 && actual_matrix[x][z] / 100 == 2 && actual_matrix[x][z+2] == 0 && actual_matrix[x][z+1] == -2 && actual_matrix[x][z-2] == 0)
            {
              return impossible;
            }
            else if(actual_matrix[x][z] / 100 == 1)
            {
              break;
            }
          }
          if(v - y == 2 && actual_matrix[x][y+1] % 10 != 3 && !queen_mandatory && !pawn_mandatory)
          {
            return normal;
          }
          else if(y+2 < 15 && actual_matrix[x][y+1] % 10 != 3 && actual_matrix[x][y+2] / 100 != 1)
          {
            if(y+4 < 15 && actual_matrix[x][y+3] % 10 != 3 && ((actual_matrix[x][y+2] == 0 && actual_matrix[x][y+4] / 100 == 2) || actual_matrix[x][y+4] == 0))
            {
              if(v - y == 4 && actual_matrix[x][y+2] == 0 && actual_matrix[x][y+4] == 0 && !queen_mandatory && !pawn_mandatory)
              {
                return normal;
              }
              else if(v - y == 4 && actual_matrix[x][y+2] / 100 == 2)
              {
                return destroy;
              }
              else if(y+6 < 15 && actual_matrix[x][y+5] % 10 != 3 && actual_matrix[x][y+6] / 100 != 1)
              {
                if(v - y == 6 && actual_matrix[x][y+4] == 0 && actual_matrix[x][y+2] == 0 && !queen_mandatory && !pawn_mandatory)
                {
                  return normal;
                }
                else if(v - y == 6 && (actual_matrix[x][y+4] / 100 == 2 || actual_matrix[x][y+2] / 100 == 2))
                {
                  return destroy;
                }
                else if(y+8 < 15 && actual_matrix[x][y+7] % 10 != 3 && ((actual_matrix[x][y+6] == 0 && actual_matrix[x][y+8] / 100 == 2) || ((actual_matrix[x][y+4] == 0 || actual_matrix[x][y+6] == 0) && actual_matrix[x][y+8] == 0)))
                {
                  if(v - y == 8 && actual_matrix[x][y+6] == 0 && actual_matrix[x][y+4] == 0 && actual_matrix[x][y+2] == 0 && !queen_mandatory && !pawn_mandatory)
                  {
                    return normal;
                  }
                  else if(v - y == 8 && (actual_matrix[x][y+6] / 100 == 2 || actual_matrix[x][y+4] / 100 == 2 || actual_matrix[x][y+2] / 100 == 2))
                  {
                    return destroy;
                  }
                  else if(y+10 < 15 && actual_matrix[x][y+9] % 10 != 3 && actual_matrix[x][y+10] / 100 != 1)
                  {
                    if(v - y == 10 && actual_matrix[x][y+8] == 0 && actual_matrix[x][y+6] == 0 && actual_matrix[x][y+4] == 0 && actual_matrix[x][y+2] == 0 && !queen_mandatory && !pawn_mandatory)
                    {
                      return normal;
                    }
                    else if(v - y == 10 && (actual_matrix[x][y+8] / 100 == 2 || actual_matrix[x][y+6] / 100 == 2 || actual_matrix[x][y+4] / 100 == 2 || actual_matrix[x][y+2] / 100 == 2))
                    {
                      return destroy;
                    }
                    else if(y+12 < 15 && actual_matrix[x][y+11] % 10 != 3 && ((actual_matrix[x][y+10] == 0 && actual_matrix[x][y+12] / 100 == 2) || ((actual_matrix[x][y+8] == 0 || actual_matrix[x][y+10] == 0) && actual_matrix[x][y+12] == 0)))
                    {
                      if(v - y == 12 && actual_matrix[x][y+10] == 0 && actual_matrix[x][y+8] == 0 && actual_matrix[x][y+6] == 0 && actual_matrix[x][y+4] == 0 && actual_matrix[x][y+2] == 0 && !queen_mandatory && !pawn_mandatory)
                      {
                        return normal;
                      }
                      else if(v - y == 12 && (actual_matrix[x][y+10] / 100 == 2 || actual_matrix[x][y+8] / 100 == 2 || actual_matrix[x][y+6] / 100 == 2 || actual_matrix[x][y+4] / 100 == 2 || actual_matrix[x][y+2] / 100 == 2))
                      {
                        return destroy;
                      }
                      else if(v - y == 14 && actual_matrix[x][y+13] % 10 != 3)
                      {
                        if(actual_matrix[x][y+12] == 0 && actual_matrix[x][y+10] == 0 && actual_matrix[x][y+8] == 0 && actual_matrix[x][y+6] == 0 && actual_matrix[x][y+4] == 0 && actual_matrix[x][y+2] == 0 && !queen_mandatory && !pawn_mandatory)
                        {
                          return normal;
                        }
                        else if(actual_matrix[x][y+12] / 100 == 2 || actual_matrix[x][y+10] / 100 == 2 || actual_matrix[x][y+8] / 100 == 2 || actual_matrix[x][y+6] / 100 == 2 || actual_matrix[x][y+4] / 100 == 2 || actual_matrix[x][y+2] / 100 == 2)
                        {
                          return destroy;
                        }
                      }
                      return impossible;
                    }
                    return impossible;
                  }
                  return impossible;
                }
                return impossible;
              }
              return impossible;
            }
            return impossible;
          }
          return impossible;
        }
        else if(y > v)
        {
          for(int z = v-2; z > 0; z -= 2)
          {
            if(actual_matrix[x][z+1] % 10 != 3 && actual_matrix[x][z] / 100 == 2 && actual_matrix[x][z-2] == 0 && actual_matrix[x][z-1] == -2 && actual_matrix[x][z+2] == 0)
            {
              return impossible;
            }
            else if(actual_matrix[x][z] / 100 == 1)
            {
              break;
            }
          }
          if(y - v == 2 && actual_matrix[x][y-1] % 10 != 3 && !queen_mandatory && !pawn_mandatory)
          {
            return normal;
          }
          else if(y-2 >= 0 && actual_matrix[x][y-1] % 10 != 3 && actual_matrix[x][y-2] / 100 != 1)
          {
            if(y-4 >= 0 && actual_matrix[x][y-3] % 10 != 3 && ((actual_matrix[x][y-2] == 0 && actual_matrix[x][y-4] / 100 == 2) || actual_matrix[x][y-4] == 0))
            {
              if(y - v == 4 && actual_matrix[x][y-2] == 0 && !queen_mandatory && !pawn_mandatory)
              {
                return normal;
              }
              else if(y - v == 4 && actual_matrix[x][y-2] / 100 == 2)
              {
                return destroy;
              }
              else if(y-6 >= 0 && actual_matrix[x][y-5] % 10 != 3 && actual_matrix[x][y-6] / 100 != 1)
              {
                if(y - v == 6 && actual_matrix[x][y-4] == 0 && actual_matrix[x][y-2] == 0 && !queen_mandatory && !pawn_mandatory)
                {
                  return normal;
                }
                else if(y - v == 6 && (actual_matrix[x][y-4] / 100 == 2 || actual_matrix[x][y-2] / 100 == 2))
                {
                  return destroy;
                }
                else if(y-8 >= 0 && actual_matrix[x][y-7] % 10 != 3 && ((actual_matrix[x][y-6] == 0 && actual_matrix[x][y-8] / 100 == 2) || ((actual_matrix[x][y-4] == 0 || actual_matrix[x][y-6] == 0) && actual_matrix[x][y-8] == 0)))
                {
                  if(y - v == 8 && actual_matrix[x][y-6] == 0 && actual_matrix[x][y-4] == 0 && actual_matrix[x][y-2] == 0 && !queen_mandatory && !pawn_mandatory)
                  {
                    return normal;
                  }
                  else if(y - v == 8 && (actual_matrix[x][y-6] / 100 == 2 || actual_matrix[x][y-4] / 100 == 2 || actual_matrix[x][y-2] / 100 == 2))
                  {
                    return destroy;
                  }
                  else if(y-10 >= 0 && actual_matrix[x][y-9] % 10 != 3 && actual_matrix[x][y-10] / 100 != 1)
                  {
                    if(y - v == 10 && actual_matrix[x][y-8] == 0 && actual_matrix[x][y-6] == 0 && actual_matrix[x][y-4] == 0 && actual_matrix[x][y-2] == 0 && !queen_mandatory && !pawn_mandatory)
                    {
                      return normal;
                    }
                    if(y - v == 10 && (actual_matrix[x][y-8] / 100 == 2 || actual_matrix[x][y-6] / 100 == 2 || actual_matrix[x][y-4] / 100 == 2 || actual_matrix[x][y-2] / 100 == 2))
                    {
                      return destroy;
                    }
                    else if(y-12 >= 0 && actual_matrix[x][y-11] % 10 != 3 && ((actual_matrix[x][y-10] == 0 && actual_matrix[x][y-12] / 100 == 2) || ((actual_matrix[x][y-8] == 0 || actual_matrix[x][y-10] == 0) && actual_matrix[x][y-12] == 0)))
                    {
                      if(y - v == 12 && actual_matrix[x][y-10] == 0 && actual_matrix[x][y-8] == 0 && actual_matrix[x][y-6] == 0 && actual_matrix[x][y-4] == 0 && actual_matrix[x][y-2] == 0 && !queen_mandatory && !pawn_mandatory)
                      {
                        return normal;
                      }
                      if(y - v == 12 && (actual_matrix[x][y-10] / 100 == 2 || actual_matrix[x][y-8] / 100 == 2 || actual_matrix[x][y-6] / 100 == 2 || actual_matrix[x][y-4] / 100 == 2 || actual_matrix[x][y-2] / 100 == 2))
                      {
                        return destroy;
                      }
                      else if(y - v == 14 && actual_matrix[x][y-13] % 10 != 3)
                      {
                        if(actual_matrix[x][y-12] == 0 && actual_matrix[x][y-10] == 0 && actual_matrix[x][y-8] == 0 && actual_matrix[x][y-6] == 0 && actual_matrix[x][y-4] == 0 && actual_matrix[x][y-2] == 0 && !queen_mandatory && !pawn_mandatory)
                        {
                          return normal;
                        }
                        else if(actual_matrix[x][y-12] / 100 == 2 || actual_matrix[x][y-10] / 100 == 2 || actual_matrix[x][y-8] / 100 == 2 || actual_matrix[x][y-6] / 100 == 2 || actual_matrix[x][y-4] / 100 == 2 || actual_matrix[x][y-2] / 100 == 2)
                        {
                          return destroy;
                        }
                      }
                      return impossible;
                    }
                    return impossible;
                  }
                  return impossible;
                }
                return impossible;
              }
              return impossible;
            }
            return impossible;
          }
          return impossible;
        }
        return impossible;
      }
      return impossible;
    }
    else if(actual_matrix[x][y] == 121)
    {
      if(abs(x - u) == 1 && abs(y - v) == 1 && actual_matrix[u][v] == 0 && !queen_mandatory && !pawn_mandatory)
      {
        return turn;
      }
      else if(x == u)
      {
        if(abs(y - v) == 2 && (/*actual_matrix[u][v] / 100 == 2 ||*/ actual_matrix[u][v] == 0) && !queen_mandatory && !pawn_mandatory)
        {
          if(v - y == 2 && actual_matrix[x][y+1] % 10 != 3)
          {
            return normal;
          }
          else if(y - v == 2 && actual_matrix[x][y-1] % 10 != 3)
          {
            return normal;
          }
          return impossible;
        }
        else if(v - y > 2 && actual_matrix[u][v] == 0 && !queen_mandatory && !pawn_mandatory)
        {
          for(int z = y+2; z <= v; z += 2)
          {
            if(actual_matrix[x][z-1] % 10 == 3 || actual_matrix[x][z] != 0)
            {
              return impossible;
            }
          }
          return normal;
        }
        else if(y - v > 2 && actual_matrix[u][v] == 0 && !queen_mandatory && !pawn_mandatory)
        {
          for(int z = y-2; z >= v; z -= 2)
          {
            if(actual_matrix[x][z+1] % 10 == 3 || actual_matrix[x][z] != 0)
            {
              return impossible;
            }
          }
          return normal;
        }
        return impossible;
      }
      else if(y == v && actual_matrix[u][v] == 0)
      {
        if(u > x)
        {
          for(int z = u+2; z < 14; z += 2)
          {
            if(actual_matrix[z-1][y] % 10 != 3 && actual_matrix[z][y] / 100 == 2 && actual_matrix[z+2][y] == 0 && actual_matrix[z+1][y] == -2 && actual_matrix[z-2][y] == 0)
            {
              return impossible;
            }
            else if(actual_matrix[z][y] / 100 == 1)
            {
              break;
            }
          }
          if(u - x == 2 && actual_matrix[x+1][y] % 10 != 3 && !queen_mandatory && !pawn_mandatory)
          {
            return normal;
          }
          else if(x+2 < 15 && actual_matrix[x+1][y] % 10 != 3 && actual_matrix[x+2][y] / 100 != 1)
          {
            if(x+4 < 15 && actual_matrix[x+3][y] % 10 != 3 && ((actual_matrix[x+2][y] == 0 && actual_matrix[x+4][y] / 100 == 2) || actual_matrix[x+4][y] == 0))
            {
              if(u - x == 4 && actual_matrix[x+2][y] == 0 && !queen_mandatory && !pawn_mandatory)
              {
                return normal;
              }
              else if(u - x == 4 && actual_matrix[x+2][y] / 100 == 2)
              {
                return destroy;
              }
              else if(x+6 < 15 && actual_matrix[x+5][y] % 10 != 3 && actual_matrix[x+6][y] / 100 != 1)
              {
                if(u - x == 6 && actual_matrix[x+4][y] == 0 && actual_matrix[x+2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                {
                  return normal;
                }
                else if(u - x == 6 && (actual_matrix[x+4][y] / 100 == 2 || actual_matrix[x+2][y] / 100 == 2))
                {
                  return destroy;
                }
                else if(x+8 < 15 && actual_matrix[x+7][y] % 10 != 3 && ((actual_matrix[x+6][y] == 0 && actual_matrix[x+8][y] / 100 == 2) || ((actual_matrix[x+4][y] == 0 || actual_matrix[x+6][y] == 0) && actual_matrix[x+8][y] == 0)))
                {
                  if(u - x == 8 && actual_matrix[x+6][y] == 0 && actual_matrix[x+4][y] == 0 && actual_matrix[x+2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                  {
                    return normal;
                  }
                  else if(u - x == 8 && (actual_matrix[x+6][y] / 100 == 2 || actual_matrix[x+4][y] / 100 == 2 || actual_matrix[x+2][y] / 100 == 2))
                  {
                    return destroy;
                  }
                  else if(x+10 < 15 && actual_matrix[x+9][y] % 10 != 3 && actual_matrix[x+10][y] / 100 != 1)
                  {
                    if(u - x == 10 && actual_matrix[x+8][y] == 0 && actual_matrix[x+6][y] == 0 && actual_matrix[x+4][y] == 0 && actual_matrix[x+2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                    {
                      return normal;
                    }
                    else if(u - x == 10 && (actual_matrix[x+8][y] / 100 == 2 || actual_matrix[x+6][y] / 100 == 2 || actual_matrix[x+4][y] / 100 == 2 || actual_matrix[x+2][y] / 100 == 2))
                    {
                      return destroy;
                    }
                    else if(x+12 < 15 && actual_matrix[x+11][y] % 10 != 3 && ((actual_matrix[x+10][y] == 0 && actual_matrix[x+12][y] / 100 == 2) || ((actual_matrix[x+8][y] == 0 || actual_matrix[x+10][y] == 0) && actual_matrix[x+12][y] == 0)))
                    {
                      if(u - x == 12 && actual_matrix[x+10][y] == 0 && actual_matrix[x+8][y] == 0 && actual_matrix[x+6][y] == 0 && actual_matrix[x+4][y] == 0 && actual_matrix[x+2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                      {
                        return normal;
                      }
                      else if(u - x == 12 && (actual_matrix[x+10][y] / 100 == 2 || actual_matrix[x+8][y] / 100 == 2 || actual_matrix[x+6][y] / 100 == 2 || actual_matrix[x+4][y] / 100 == 2 || actual_matrix[x+2][y] / 100 == 2))
                      {
                        return destroy;
                      }
                      else if(u - x == 14 && actual_matrix[x+13][y] % 10 != 3)
                      {
                        if(actual_matrix[x+12][y] == 0 && actual_matrix[x+10][y] == 0 && actual_matrix[x+8][y] == 0 && actual_matrix[x+6][y] == 0 && actual_matrix[x+4][y] == 0 && actual_matrix[x+2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                        {
                          return normal;
                        }
                        else if(actual_matrix[x+12][y] / 100 == 2 || actual_matrix[x+10][y] / 100 == 2 || actual_matrix[x+8][y] / 100 == 2 || actual_matrix[x+6][y] / 100 == 2 || actual_matrix[x+4][y] / 100 == 2 || actual_matrix[x+2][y] / 100 == 2)
                        {
                          return destroy;
                        }
                      }
                      return impossible;
                    }
                    return impossible;
                  }
                  return impossible;
                }
                return impossible;
              }
              return impossible;
            }
            return impossible;
          }
          return impossible;
        }
        else if(x > u)
        {
          for(int z = u-2; z > 0; z -= 2)
          {
            if(actual_matrix[z+1][y] % 10 != 3 && actual_matrix[z][y] / 100 == 2 && actual_matrix[z-2][y] == 0 && actual_matrix[z-1][y] == -2 && actual_matrix[z+2][y] == 0)
            {
              return impossible;
            }
            else if(actual_matrix[z][y] / 100 == 1)
            {
              break;
            }
          }
          if(x - u == 2 && actual_matrix[x-1][y] % 10 != 3 && !queen_mandatory && !pawn_mandatory)
          {
            return normal;
          }
          else if(x-2 >= 0 && actual_matrix[x-1][y] % 10 != 3 && actual_matrix[x-2][y] / 100 != 1)
          {
            if(x-4 >= 0 && actual_matrix[x-3][y] % 10 != 3 && ((actual_matrix[x-2][y] == 0 && actual_matrix[x-4][y] / 100 == 2) || actual_matrix[x-4][y] == 0))
            {
              if(x - u == 4 && actual_matrix[x-2][y] == 0 && !queen_mandatory && !pawn_mandatory)
              {
                return normal;
              }
              else if(x - u == 4 && actual_matrix[x-2][y] / 100 == 2)
              {
                return destroy;
              }
              else if(x-6 >= 0 && actual_matrix[x-5][y] % 10 != 3 && actual_matrix[x-6][y] / 100 != 1)
              {
                if(x - u == 6 && actual_matrix[x-4][y] == 0 && actual_matrix[x-2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                {
                  return normal;
                }
                else if(x - u == 6 && (actual_matrix[x-4][y] / 100 == 2 || actual_matrix[x-2][y] / 100 == 2))
                {
                  return destroy;
                }
                else if(x-8 >= 0 && actual_matrix[x-7][y] % 10 != 3 && ((actual_matrix[x-6][y] == 0 && actual_matrix[x-8][y] / 100 == 2) || ((actual_matrix[x-4][y] == 0 || actual_matrix[x-6][y] == 0) && actual_matrix[x-8][y] == 0)))
                {
                  if(x - u == 8 && actual_matrix[x-6][y] == 0 && actual_matrix[x-4][y] == 0 && actual_matrix[x-2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                  {
                    return normal;
                  }
                  else if(x - u == 8 && (actual_matrix[x-6][y] / 100 == 2 || actual_matrix[x-4][y] / 100 == 2 || actual_matrix[x-2][y] / 100 == 2))
                  {
                    return destroy;
                  }
                  else if(x-10 >= 0 && actual_matrix[x-9][y] % 10 != 3 && actual_matrix[x-10][y] / 100 != 1)
                  {
                    if(x - u == 10 && actual_matrix[x-8][y] == 0 && actual_matrix[x-6][y] == 0 && actual_matrix[x-4][y] == 0 && actual_matrix[x-2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                    {
                      return normal;
                    }
                    else if(x - u == 10 && (actual_matrix[x-8][y] / 100 == 2 || actual_matrix[x-6][y] / 100 == 2 || actual_matrix[x-4][y] / 100 == 2 || actual_matrix[x-2][y] / 100 == 2))
                    {
                      return destroy;
                    }
                    else if(x-12 >= 0 && actual_matrix[x-11][y] % 10 != 3 && ((actual_matrix[x-10][y] == 0 && actual_matrix[x-12][y] / 100 == 2) || ((actual_matrix[x-8][y] == 0 || actual_matrix[x-10][y] == 0) && actual_matrix[x-12][y] == 0)))
                    {
                      if(x - u == 12 && actual_matrix[x-10][y] == 0 && actual_matrix[x-8][y] == 0 && actual_matrix[x-6][y] == 0 && actual_matrix[x-4][y] == 0 && actual_matrix[x-2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                      {
                        return normal;
                      }
                      else if(x - u == 12 && (actual_matrix[x-10][y] / 100 == 2 || actual_matrix[x-8][y] / 100 == 2 || actual_matrix[x-6][y] / 100 == 2 || actual_matrix[x-4][y] / 100 == 2 || actual_matrix[x-2][y] / 100 == 2))
                      {
                        return destroy;
                      }
                      else if(x - u == 14 && actual_matrix[x-13][y] % 10 != 3)
                      {
                        if(actual_matrix[x-12][y] == 0 && actual_matrix[x-10][y] == 0 && actual_matrix[x-8][y] == 0 && actual_matrix[x-6][y] == 0 && actual_matrix[x-4][y] == 0 && actual_matrix[x-2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                        {
                          return normal;
                        }
                        else if(actual_matrix[x-12][y] / 100 == 2 || actual_matrix[x-10][y] / 100 == 2 || actual_matrix[x-8][y] / 100 == 2 || actual_matrix[x-6][y] / 100 == 2 || actual_matrix[x-4][y] / 100 == 2 || actual_matrix[x-2][y] / 100 == 2)
                        {
                          return destroy;
                        }
                      }
                      return impossible;
                    }
                    return impossible;
                  }
                  return impossible;
                }
                return impossible;
              }
              return impossible;
            }
            return impossible;
          }
          return impossible;
        }
        return impossible;
      }
      return impossible;
    }
    return impossible;
  }
  else if(actual_matrix[x][y] == 220 || actual_matrix[x][y] == 221)
  {
    if(actual_matrix[x][y] == 220)
    {
      if(abs(x - u) == 1 && abs(y - v) == 1 && actual_matrix[u][v] == 0 && !queen_mandatory && !pawn_mandatory)
      {
        return turn;
      }
      else if(y == v)
      {
        if(abs(u - x) == 2 && (/*actual_matrix[u][v] / 100 == 1 ||*/ actual_matrix[u][v] == 0) && !pawn_mandatory && !queen_mandatory)
        {
          if(u - x == 2 && actual_matrix[x+1][y] % 10 != 3)
          {
            return normal;
          }
          else if(x - u == 2 && actual_matrix[x-1][y] % 10 != 3)
          {
            return normal;
          }
          return impossible;
        }
        else if(u - x > 2 && actual_matrix[u][v] == 0 && !queen_mandatory && !pawn_mandatory)
        {
          for(int z = x+2; z <= u; z += 2)
          {
            if(actual_matrix[z-1][y] % 10 == 3 || actual_matrix[z][y] != 0)
            {
              return impossible;
            }
          }
          return normal;
        }
        else if(x - u > 2 && actual_matrix[u][v] == 0 && !queen_mandatory && !pawn_mandatory)
        {
          for(int z = x-2; z >= u; z -= 2)
          {
            if(actual_matrix[z+1][y] % 10 == 3 || actual_matrix[z][y] != 0)
            {
              return impossible;
            }
          }
          return normal;
        }
        return impossible;
      }
      else if(x == u && actual_matrix[u][v] == 0)
      {
        if(v > y)
        {
          for(int z = v+2; z < 14; z += 2)
          {
            if(actual_matrix[x][z-1] % 10 != 3 && actual_matrix[x][z] / 100 == 1 && actual_matrix[x][z+2] == 0 && actual_matrix[x][z+1] == -2 && actual_matrix[x][z-2] == 0)
            {
              return impossible;
            }
            else if(actual_matrix[x][z] / 100 == 2)
            {
              break;
            }
          }
          if(v - y == 2 && actual_matrix[x][y+1] % 10 != 3 && !queen_mandatory && !pawn_mandatory)
          {
            return normal;
          }
          else if(y+2 < 15 && actual_matrix[x][y+1] % 10 != 3 && actual_matrix[x][y+2] / 100 != 2)
          {
            if(y+4 < 15 && actual_matrix[x][y+3] % 10 != 3 && ((actual_matrix[x][y+2] == 0 && actual_matrix[x][y+4] / 100 == 1) || actual_matrix[x][y+4] == 0))
            {
              if(v - y == 4 && actual_matrix[x][y+2] == 0 && !queen_mandatory && !pawn_mandatory)
              {
                return normal;
              }
              else if(v - y == 4 && actual_matrix[x][y+2] / 100 == 1)
              {
                return destroy;
              }
              else if(y+6 < 15 && actual_matrix[x][y+5] % 10 != 3 && actual_matrix[x][y+6] / 100 != 2)
              {
                if(v - y == 6 && actual_matrix[x][y+4] == 0 && actual_matrix[x][y+2] == 0 && !queen_mandatory && !pawn_mandatory)
                {
                  return normal;
                }
                else if(v - y == 6 && (actual_matrix[x][y+4] / 100 == 1 || actual_matrix[x][y+2] / 100 == 1))
                {
                  return destroy;
                }
                else if(y+8 < 15 && actual_matrix[x][y+7] % 10 != 3 && ((actual_matrix[x][y+6] == 0 && actual_matrix[x][y+8] / 100 == 1) || ((actual_matrix[x][y+4] == 0 || actual_matrix[x][y+6] == 0) && actual_matrix[x][y+8] == 0)))
                {
                  if(v - y == 8 && actual_matrix[x][y+6] == 0 && actual_matrix[x][y+4] == 0 && actual_matrix[x][y+2] == 0 && !queen_mandatory && !pawn_mandatory)
                  {
                    return normal;
                  }
                  else if(v - y == 8 && (actual_matrix[x][y+6] / 100 == 1 || actual_matrix[x][y+4] / 100 == 1 || actual_matrix[x][y+2] / 100 == 1))
                  {
                    return destroy;
                  }
                  else if(y+10 < 15 && actual_matrix[x][y+9] % 10 != 3 && actual_matrix[x][y+10] / 100 != 2)
                  {
                    if(v - y == 10 && actual_matrix[x][y+8] == 0 && actual_matrix[x][y+6] == 0 && actual_matrix[x][y+4] == 0 && actual_matrix[x][y+2] == 0 && !queen_mandatory && !pawn_mandatory)
                    {
                      return normal;
                    }
                    else if(v - y == 10 && (actual_matrix[x][y+8] / 100 == 1 || actual_matrix[x][y+6] / 100 == 1 || actual_matrix[x][y+4] / 100 == 1 || actual_matrix[x][y+2] / 100 == 1))
                    {
                      return destroy;
                    }
                    else if(y+12 < 15 && actual_matrix[x][y+11] % 10 != 3 && ((actual_matrix[x][y+10] == 0 && actual_matrix[x][y+12] / 100 == 1) || ((actual_matrix[x][y+8] == 0 || actual_matrix[x][y+10] == 0) && actual_matrix[x][y+12] == 0)))
                    {
                      if(v - y == 12 && actual_matrix[x][y+10] == 0 && actual_matrix[x][y+8] == 0 && actual_matrix[x][y+6] == 0 && actual_matrix[x][y+4] == 0 && actual_matrix[x][y+2] == 0 && !queen_mandatory && !pawn_mandatory)
                      {
                        return normal;
                      }
                      else if(v - y == 12 && (actual_matrix[x][y+10] / 100 == 1 || actual_matrix[x][y+8] / 100 == 1 || actual_matrix[x][y+6] / 100 == 1 || actual_matrix[x][y+4] / 100 == 1 || actual_matrix[x][y+2] / 100 == 1))
                      {
                        return destroy;
                      }
                      else if(v - y == 14 && actual_matrix[x][y+13] % 10 != 3)
                      {
                        if(actual_matrix[x][y+12] == 0 && actual_matrix[x][y+10] == 0 && actual_matrix[x][y+8] == 0 && actual_matrix[x][y+6] == 0 && actual_matrix[x][y+4] == 0 && actual_matrix[x][y+2] == 0 && !queen_mandatory && !pawn_mandatory)
                        {
                          return normal;
                        }
                        else if(actual_matrix[x][y+12] / 100 == 1 || actual_matrix[x][y+10] / 100 == 1 || actual_matrix[x][y+8] / 100 == 1 || actual_matrix[x][y+6] / 100 == 1 || actual_matrix[x][y+4] / 100 == 1 || actual_matrix[x][y+2] / 100 == 1)
                        {
                          return destroy;
                        }
                      }
                      return impossible;
                    }
                    return impossible;
                  }
                  return impossible;
                }
                return impossible;
              }
              return impossible;
            }
            return impossible;
          }
          return impossible;
        }
        else if(y > v)
        {
          for(int z = v-2; z > 0; z -= 2)
          {
            if(actual_matrix[x][z+1] % 10 != 3 && actual_matrix[x][z] / 100 == 1 && actual_matrix[x][z-2] == 0 && actual_matrix[x][z-1] == -2 && actual_matrix[x][z+2] == 0)
            {
              return impossible;
            }
            else if(actual_matrix[x][z] / 100 == 2)
            {
              break;
            }
          }
          if(y - v == 2 && actual_matrix[x][y-1] % 10 != 3 && !queen_mandatory && !pawn_mandatory)
          {
            return normal;
          }
          else if(y-2 >= 0 && actual_matrix[x][y-1] % 10 != 3 && actual_matrix[x][y-2] / 100 != 2)
          {
            if(y-4 >= 0 && actual_matrix[x][y-3] % 10 != 3 && ((actual_matrix[x][y-2] == 0 && actual_matrix[x][y-4] / 100 == 1) || actual_matrix[x][y-4] == 0))
            {
              if(y - v == 4 && actual_matrix[x][y-2] == 0 && !queen_mandatory && !pawn_mandatory)
              {
                return normal;
              }
              else if(y - v == 4 && actual_matrix[x][y-2] / 100 == 1)
              {
                return destroy;
              }
              else if(y-6 >= 0 && actual_matrix[x][y-5] % 10 != 3 && actual_matrix[x][y-6] / 100 != 2)
              {
                if(y - v == 6 && actual_matrix[x][y-4] == 0 && actual_matrix[x][y-2] == 0 && !queen_mandatory && !pawn_mandatory)
                {
                  return normal;
                }
                else if(y - v == 6 && (actual_matrix[x][y-4] / 100 == 1 || actual_matrix[x][y-2] / 100 == 1))
                {
                  return destroy;
                }
                else if(y-8 >= 0 && actual_matrix[x][y-7] % 10 != 3 && ((actual_matrix[x][y-6] == 0 && actual_matrix[x][y-8] / 100 == 1) || ((actual_matrix[x][y-4] == 0 || actual_matrix[x][y-6] == 0) && actual_matrix[x][y-8] == 0)))
                {
                  if(y - v == 8 && actual_matrix[x][y-6] == 0 && actual_matrix[x][y-4] == 0 && actual_matrix[x][y-2] == 0 && !queen_mandatory && !pawn_mandatory)
                  {
                    return normal;
                  }
                  else if(y - v == 8 && (actual_matrix[x][y-6] / 100 == 1 || actual_matrix[x][y-4] / 100 == 1 || actual_matrix[x][y-2] / 100 == 1))
                  {
                    return destroy;
                  }
                  else if(y-10 >= 0 && actual_matrix[x][y-9] % 10 != 3 && actual_matrix[x][y-10] / 100 != 2)
                  {
                    if(y - v == 10 && actual_matrix[x][y-8] == 0 && actual_matrix[x][y-6] == 0 && actual_matrix[x][y-4] == 0 && actual_matrix[x][y-2] == 0 && !queen_mandatory && !pawn_mandatory)
                    {
                      return normal;
                    }
                    else if(y - v == 10 && (actual_matrix[x][y-8] / 100 == 1 || actual_matrix[x][y-6] / 100 == 1 || actual_matrix[x][y-4] / 100 == 1 || actual_matrix[x][y-2] / 100 == 1))
                    {
                      return destroy;
                    }
                    else if(y-12 >= 0 && actual_matrix[x][y-11] % 10 != 3 && ((actual_matrix[x][y-10] == 0 && actual_matrix[x][y-12] / 100 == 1) || ((actual_matrix[x][y-8] == 0 || actual_matrix[x][y-6] == 0) && actual_matrix[x][y-12] == 0)))
                    {
                      if(y - v == 12 && actual_matrix[x][y-10] == 0 && actual_matrix[x][y-8] == 0 && actual_matrix[x][y-6] == 0 && actual_matrix[x][y-4] == 0 && actual_matrix[x][y-2] == 0 && !queen_mandatory && !pawn_mandatory)
                      {
                        return normal;
                      }
                      else if(y - v == 12 && (actual_matrix[x][y-10] / 100 == 1 || actual_matrix[x][y-8] / 100 == 1 || actual_matrix[x][y-6] / 100 == 1 || actual_matrix[x][y-4] / 100 == 1 || actual_matrix[x][y-2] / 100 == 1))
                      {
                        return destroy;
                      }
                      else if(y - v == 14 && actual_matrix[x][y-13] % 10 != 3)
                      {
                        if(actual_matrix[x][y-12] == 0 && actual_matrix[x][y-10] == 0 && actual_matrix[x][y-8] == 0 && actual_matrix[x][y-6] == 0 && actual_matrix[x][y-4] == 0 && actual_matrix[x][y-2] == 0 && !queen_mandatory && !pawn_mandatory)
                        {
                          return normal;
                        }
                        else if(actual_matrix[x][y-12] / 100 == 1 || actual_matrix[x][y-10] / 100 == 1 || actual_matrix[x][y-8] / 100 == 1 || actual_matrix[x][y-6] / 100 == 1 || actual_matrix[x][y-4] / 100 == 1 || actual_matrix[x][y-2] / 100 == 1)
                        {
                          return destroy;
                        }
                      }
                      return impossible;
                    }
                    return impossible;
                  }
                  return impossible;
                }
                return impossible;
              }
              return impossible;
            }
            return impossible;
          }
          return impossible;
        }
        return impossible;
      }
      return impossible;
    }
    else if(actual_matrix[x][y] == 221)
    {
      if(abs(x - u) == 1 && abs(y - v) == 1 && actual_matrix[u][v] == 0 && !pawn_mandatory && !queen_mandatory)
      {
        return turn;
      }
      else if(x == u)
      {
        if(abs(y - v) == 2 && (/*actual_matrix[u][v] / 100 == 1 ||*/ actual_matrix[u][v] == 0) && !queen_mandatory && !pawn_mandatory)
        {
          if(v - y == 2 && actual_matrix[x][y+1] % 10 != 3)
          {
            return normal;
          }
          else if(y - v == 2 && actual_matrix[x][y-1] % 10 != 3)
          {
            return normal;
          }
          return impossible;
        }
        else if(v - y > 2 && actual_matrix[u][v] == 0 && !queen_mandatory && !pawn_mandatory)
        {
          for(int z = y+2; z <= v; z += 2)
          {
            if(actual_matrix[x][z-1] % 10 == 3 || actual_matrix[x][z] != 0)
            {
              return impossible;
            }
          }
          return normal;
        }
        else if(y - v > 2 && actual_matrix[u][v] == 0 && !queen_mandatory && !pawn_mandatory)
        {
          for(int z = y-2; z >= v; z -= 2)
          {
            if(actual_matrix[x][z+1] % 10 == 3 || actual_matrix[x][z] != 0)
            {
              return impossible;
            }
          }
          return normal;
        }
        return impossible;
      }
      else if(y == v && actual_matrix[u][v] == 0)
      {
        if(u > x)
        {
          for(int z = u+2; z < 14; z += 2)
          {
            if(actual_matrix[z-1][y] % 10 != 3 && actual_matrix[z][y] / 100 == 1 && actual_matrix[z+2][y] == 0 && actual_matrix[z+1][y] == -2 && actual_matrix[z-2][y] == 0)
            {
              return impossible;
            }
            else if(actual_matrix[z][y] / 100 == 2)
            {
              break;
            }
          }
          if(u - x == 2 && actual_matrix[x+1][y] % 10 != 3 && !queen_mandatory && !pawn_mandatory)
          {
            return normal;
          }
          else if(x+2 < 15 && actual_matrix[x+1][y] % 10 != 3 && actual_matrix[x+2][y] / 100 != 2)
          {
            if(x+4 < 15 && actual_matrix[x+3][y] % 10 != 3 && ((actual_matrix[x+2][y] == 0 && actual_matrix[x+4][y] / 100 == 1) || actual_matrix[x+4][y] == 0))
            {
              if(u - x == 4 && actual_matrix[x+2][y] == 0 && !queen_mandatory && !pawn_mandatory)
              {
                return normal;
              }
              else if(u - x == 4 && actual_matrix[x+2][y] / 100 == 1)
              {
                return destroy;
              }
              else if(x+6 < 15 && actual_matrix[x+5][y] % 10 != 3 && actual_matrix[x+6][y] / 100 != 2)
              {
                if(u - x == 6 && actual_matrix[x+4][y] == 0 && actual_matrix[x+2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                {
                  return normal;
                }
                else if(u - x == 6 && (actual_matrix[x+4][y] / 100 == 1 || actual_matrix[x+2][y] / 100 == 1))
                {
                  return destroy;
                }
                else if(x+8 < 15 && actual_matrix[x+7][y] % 10 != 3 && ((actual_matrix[x+6][y] == 0 && actual_matrix[x+8][y] / 100 == 1) || ((actual_matrix[x+4][y] == 0 || actual_matrix[x+6][y] == 0) && actual_matrix[x+8][y] == 0)))
                {
                  if(u - x == 8 && actual_matrix[x+6][y] == 0 && actual_matrix[x+4][y] == 0 && actual_matrix[x+2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                  {
                    return normal;
                  }
                  if(u - x == 8 && (actual_matrix[x+6][y] / 100 == 1 || actual_matrix[x+4][y] / 100 == 1 || actual_matrix[x+2][y] / 100 == 1))
                  {
                    return destroy;
                  }
                  else if(x+10 < 15 && actual_matrix[x+9][y] % 10 != 3 && actual_matrix[x+10][y] / 100 != 2)
                  {
                    if(u - x == 10 && actual_matrix[x+8][y] == 0 && actual_matrix[x+6][y] == 0 && actual_matrix[x+4][y] == 0 && actual_matrix[x+2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                    {
                      return normal;
                    }
                    else if(u - x == 10 && (actual_matrix[x+8][y] / 100 == 1 || actual_matrix[x+6][y] / 100 == 1 || actual_matrix[x+4][y] / 100 == 1 || actual_matrix[x+2][y] / 100 == 1))
                    {
                      return destroy;
                    }
                    else if(x+12 < 15 && actual_matrix[x+11][y] % 10 != 3 && ((actual_matrix[x+10][y] == 0 && actual_matrix[x+12][y] / 100 == 1) || ((actual_matrix[x+8][y] == 0 || actual_matrix[x+10][y] == 0) && actual_matrix[x+12][y] == 0)))
                    {
                      if(u - x == 12 && actual_matrix[x+10][y] == 0 && actual_matrix[x+8][y] == 0 && actual_matrix[x+6][y] == 0 && actual_matrix[x+4][y] == 0 && actual_matrix[x+2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                      {
                        return normal;
                      }
                      else if(u - x == 12 && (actual_matrix[x+10][y] / 100 == 1 || actual_matrix[x+8][y] / 100 == 1 || actual_matrix[x+6][y] / 100 == 1 || actual_matrix[x+4][y] / 100 == 1 || actual_matrix[x+2][y] / 100 == 1))
                      {
                        return destroy;
                      }
                      else if(u - x == 14 && actual_matrix[x+13][y] % 10 != 3)
                      {
                        if(actual_matrix[x+12][y] == 0 && actual_matrix[x+10][y] == 0 && actual_matrix[x+8][y] == 0 && actual_matrix[x+6][y] == 0 && actual_matrix[x+4][y] == 0 && actual_matrix[x+2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                        {
                          return normal;
                        }
                        else if(actual_matrix[x+12][y] / 100 == 1 || actual_matrix[x+10][y] / 100 == 1 || actual_matrix[x+8][y] / 100 == 1 || actual_matrix[x+6][y] / 100 == 1 || actual_matrix[x+4][y] / 100 == 1 || actual_matrix[x+2][y] / 100 == 1)
                        {
                          return destroy;
                        }
                      }
                      return impossible;
                    }
                    return impossible;
                  }
                  return impossible;
                }
                return impossible;
              }
              return impossible;
            }
            return impossible;
          }
          return impossible;
        }
        else if(x > u)
        {
          for(int z = u-2; z > 0; z -= 2)
          {
            if(actual_matrix[z+1][y] == -2 && actual_matrix[z][y] / 100 == 1 && actual_matrix[z-2][y] == 0 && actual_matrix[z-1][y] == -2 && actual_matrix[z+2][y] == 0)
            {
              return impossible;
            }
            else if(actual_matrix[z][y] / 100 == 2)
            {
              break;
            }
          }
          if(x - u == 2 && actual_matrix[x-1][y] == -2 && !queen_mandatory && !pawn_mandatory)
          {
            return normal;
          }
          else if(x-2 >= 0 && actual_matrix[x-1][y] % 10 != 3 && actual_matrix[x-2][y] / 100 != 2)
          {
            if(x-4 >= 0 && actual_matrix[x-3][y] % 10 != 3 && ((actual_matrix[x-2][y] == 0 && actual_matrix[x-4][y] / 100 == 1) || actual_matrix[x-4][y] == 0))
            {
              if(x - u == 4 && actual_matrix[x-2][y] == 0 && !queen_mandatory && !pawn_mandatory)
              {
                return normal;
              }
              else if(x - u == 4 && actual_matrix[x-2][y] / 100 == 1)
              {
                return destroy;
              }
              else if(x-6 >= 0 && actual_matrix[x-5][y] % 10 != 3 && actual_matrix[x-6][y] / 100 != 2)
              {
                if(x - u == 6 && actual_matrix[x-4][y] == 0 && actual_matrix[x-2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                {
                  return normal;
                }
                else if(x - u == 6 && (actual_matrix[x-4][y] / 100 == 1 || actual_matrix[x-2][y] / 100 == 1))
                {
                  return destroy;
                }
                else if(x-8 >= 0 && actual_matrix[x-7][y] % 10 != 3 && ((actual_matrix[x-6][y] == 0 && actual_matrix[x-8][y] / 100 == 1) || ((actual_matrix[x-4][y] == 0 || actual_matrix[x-6][y] == 0) && actual_matrix[x-8][y] == 0)))
                {
                  if(x - u == 8 && actual_matrix[x-6][y] == 0 && actual_matrix[x-4][y] == 0 && actual_matrix[x-2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                  {
                    return normal;
                  }
                  else if(x - u == 8 && (actual_matrix[x-6][y] / 100 == 1 || actual_matrix[x-4][y] / 100 == 1 || actual_matrix[x-2][y] / 100 == 1))
                  {
                    return destroy;
                  }
                  else if(x-10 >= 0 && actual_matrix[x-9][y] % 10 != 3 && actual_matrix[x-10][y] / 100 != 2)
                  {
                    if(x - u == 10 && actual_matrix[x-8][y] == 0 && actual_matrix[x-6][y] == 0 && actual_matrix[x-4][y] == 0 && actual_matrix[x-2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                    {
                      return normal;
                    }
                    else if(x - u == 10 && (actual_matrix[x-8][y] / 100 == 1 || actual_matrix[x-6][y] / 100 == 1 || actual_matrix[x-4][y] / 100 == 1 || actual_matrix[x-2][y] / 100 == 1))
                    {
                      return destroy;
                    }
                    else if(x-12 >= 0 && actual_matrix[x-11][y] % 10 != 3 && ((actual_matrix[x-10][y] == 0 && actual_matrix[x-12][y] / 100 == 1) || ((actual_matrix[x-8][y] == 0 || actual_matrix[x-10][y] == 0) && actual_matrix[x-12][y] == 0)))
                    {
                      if(x - u == 12 && actual_matrix[x-10][y] == 0 && actual_matrix[x-8][y] == 0 && actual_matrix[x-6][y] == 0 && actual_matrix[x-4][y] == 0 && actual_matrix[x-2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                      {
                        return normal;
                      }
                      if(x - u == 12 && (actual_matrix[x-10][y] / 100 == 1 || actual_matrix[x-8][y] / 100 == 1 || actual_matrix[x-6][y] / 100 == 1 || actual_matrix[x-4][y] / 100 == 1 || actual_matrix[x-2][y] / 100 == 1))
                      {
                        return destroy;
                      }
                      else if(x - u == 14 && actual_matrix[x-13][y] % 10 != 3)
                      {
                        if(actual_matrix[x-12][y] == 0 && actual_matrix[x-10][y] == 0 && actual_matrix[x-8][y] == 0 && actual_matrix[x-6][y] == 0 && actual_matrix[x-4][y] == 0 && actual_matrix[x-2][y] == 0 && !queen_mandatory && !pawn_mandatory)
                        {
                          return normal;
                        }
                        else if(actual_matrix[x-12][y] / 100 == 1 || actual_matrix[x-10][y] / 100 == 1 || actual_matrix[x-8][y] / 100 == 1 || actual_matrix[x-6][y] / 100 == 1 || actual_matrix[x-4][y] / 100 == 1 || actual_matrix[x-2][y] / 100 == 1)
                        {
                          return destroy;
                        }
                      }
                      return impossible;
                    }
                    return impossible;
                  }
                  return impossible;
                }
                return impossible;
              }
              return impossible;
            }
            return impossible;
          }
          return impossible;
        }
        return impossible;
      }
      return impossible;
    }
    return impossible;
  }
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
  else if(apply == destroy110 || apply == destroy111 || apply == destroy_max11)
  {
    (*actual_matrix)[x+2][y] = 0;
    (*actual_matrix)[x+4][y] = (*actual_matrix)[x][y];
    (*actual_matrix)[x][y] = 0;
    if(apply == destroy111 || apply == destroy_max11)
    {
      draw(*actual_matrix);
      SDL_Delay(200);
      (*actual_matrix)[x+4][y] = 0;
      SDL_Delay(200);
      (*actual_matrix)[x+6][y] = 0;
      (*actual_matrix)[x+8][y] = temp;
      playSound(sMove[1]);
    }
    if(apply == destroy_max11)
    {
      draw(*actual_matrix);
      SDL_Delay(200);
      (*actual_matrix)[x+8][y] = 0;
      SDL_Delay(200);
      (*actual_matrix)[x+10][y] = 0;
      (*actual_matrix)[x+12][y] = temp;
      playSound(sMove[1]);
    }
    pawn_mandatory = false;
  }
  else if(apply == destroy210 || apply == destroy211 || apply == destroy_max21)
  {
    (*actual_matrix)[x-2][y] = 0;
    (*actual_matrix)[x-4][y] = (*actual_matrix)[x][y];
    (*actual_matrix)[x][y] = 0;
    if(apply == destroy211 || apply == destroy_max21)
    {
      draw(*actual_matrix);
      SDL_Delay(200);
      (*actual_matrix)[x-4][y] = 0;
      SDL_Delay(200);
      (*actual_matrix)[x-6][y] = 0;
      (*actual_matrix)[x-8][y] = temp;
      playSound(sMove[1]);
    }
    if(apply == destroy_max21)
    {
      draw(*actual_matrix);
      SDL_Delay(200);
      (*actual_matrix)[x-8][y] = 0;
      SDL_Delay(200);
      (*actual_matrix)[x-10][y] = 0;
      (*actual_matrix)[x-12][y] = temp;
      playSound(sMove[1]);
    }
    pawn_mandatory = false;
  }
  else if(apply == destroy)
  {
    (*actual_matrix)[x][y] = 0;
    SDL_Delay(100);
    if(x == u)
    {
      if(v > y)
      {
        for(int z = y+2; z < v; z += 2)
        {
          if((*actual_matrix)[x][z] / 100 != temp / 100 && (*actual_matrix)[x][z] != 0)
          {
            if(z != y+2 && z != y+4)
            {
              SDL_Delay(400);
              playSound(sMove[1]);
            }
            (*actual_matrix)[x][z-2] = 0;
            (*actual_matrix)[x][z] = 0;
            (*actual_matrix)[x][z+2] = temp;
            draw(*actual_matrix);
          }
          else if((*actual_matrix)[x][z] == temp)
          {
            (*actual_matrix)[x][z] = 0;
          }
        }
      }
      else if(y > v)
      {
        for(int z = y-2; z > v; z -= 2)
        {
          if((*actual_matrix)[x][z] / 100 != temp / 100 && (*actual_matrix)[x][z] != 0)
          {
            if(z != y-2 && z != y-4)
            {
              SDL_Delay(400);
              playSound(sMove[1]);
            }
            (*actual_matrix)[x][z+2] = 0;
            (*actual_matrix)[x][z] = 0;
            (*actual_matrix)[x][z-2] = temp;
            draw(*actual_matrix);
          }
          else if((*actual_matrix)[x][z] == temp)
          {
            (*actual_matrix)[x][z] = 0;
          }
        }
      }
    }
    else if(y == v)
    {
      if(u > x)
      {
        for(int z = x+2; z < u; z += 2)
        {
          if((*actual_matrix)[z][y] / 100 != temp / 100 && (*actual_matrix)[z][y] != 0)
          {
            if(z != x+2 && z != x+4)
            {
              SDL_Delay(400);
              playSound(sMove[1]);
            }
            (*actual_matrix)[z-2][y] = 0;
            (*actual_matrix)[z][y] = 0;
            (*actual_matrix)[z+2][y] = temp;
            draw(*actual_matrix);
          }
          else if((*actual_matrix)[z][y] == temp)
          {
            (*actual_matrix)[z][y] = 0;
          }
        }
      }
      else if(x > u)
      {
        for(int z = x-2; z > u; z -= 2)
        {
          if((*actual_matrix)[z][y] / 100 != temp / 100 && (*actual_matrix)[z][y] != 0)
          {
            if(z != x-2 && z != x-4)
            {
              SDL_Delay(400);
              playSound(sMove[1]);
            }
            (*actual_matrix)[z+2][y] = 0;
            (*actual_matrix)[z][y] = 0;
            (*actual_matrix)[z-2][y] = temp;
            draw(*actual_matrix);
          }
          else if((*actual_matrix)[z][y] == temp)
          {
            (*actual_matrix)[z][y] = 0;
          }
        }
      }
    }
    if(((u == 14 || u == 0) && y == v) || ((v == 14 || v == 0) && x == u))
    {
      SDL_Delay(400);
    }
    (*actual_matrix)[u][v] = temp;
    queen_mandatory = false;
  }
  if(apply != impossible)
  {
    if(temp / 10 == 11 && u == 14)
    {
      draw(*actual_matrix);
      SDL_Delay(600);
      playSound(sPup);
      (*actual_matrix)[u][v] = 121;
    }
    else if(temp / 10 == 21 && u == 0)
    {
      draw(*actual_matrix);
      SDL_Delay(600);
      playSound(sPup);
      (*actual_matrix)[u][v] = 221;
    }
    actual_player++;
  }
}
