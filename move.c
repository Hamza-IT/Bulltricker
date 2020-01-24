#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "move.h"
#include "init_mat.h"
#include "gui.h"

int actual_player = 0;

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
  else if(actual_player == 0)
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
  }
  else if(actual_matrix[x][y] == 23)
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
  else if(actual_matrix[x][y] == 13)
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
  else if(actual_matrix[x][y] == 110 || actual_matrix[x][y] == 111)
  {
    if(y == v)
    {
      if(y % 2 == 1 && actual_matrix[u][v] == 0)
      {
        if(u - x == 2 && actual_matrix[x+1][v] % 10 != 3)
        {
          return normal;
        }
        else if(x + 2 < 15 && actual_matrix[x+2][y] / 100 == 2 && u - x > 2 && actual_matrix[x+1][y] % 10 != 3)
        {
          if(x + 4 < 15 && actual_matrix[x+4][y] == 0 && actual_matrix[x+3][y] % 10 != 3)
          {
            if(((x + 6 < 15 && actual_matrix[x+6][y] / 100 != 2) || (x + 6 >= 15) || (actual_matrix[x+5][y] % 10 == 3)) && u - x == 4)
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
                if(((x + 10 < 15 && actual_matrix[x+10][y] / 100 != 2) || (x + 10 >= 15) || (actual_matrix[x+9][y] % 10 == 3)) && u - x == 8)
                {
                  return destroy111;
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
        if(u - x == 2 && actual_matrix[u][v] == 0)
        {
          return normal;
        }
        else if(x + 2 < 15 && actual_matrix[x+2][y] / 100 == 2 && u - x == 2)
        {
          return normal;
        }
        return impossible;
      }
    }
    else if(abs(y - v) == 1 && u - x == 1)
    {
      if(actual_matrix[u][v] == 0)
      {
        return turn;
      }
      return impossible;
    }
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
          return normal;
        }
        else if(x - 2 >= 0 && actual_matrix[x-2][y] / 100 == 1 && x - u > 2 && actual_matrix[x-1][y] % 10 != 3)
        {
          if(x - 4 >= 0 && actual_matrix[x-4][y] == 0 && actual_matrix[x-3][y] % 10 != 3)
          {
            if(((x - 6 >= 0 && actual_matrix[x-6][y] / 100 != 1) || (x - 6 < 0) || (actual_matrix[x-5][y] % 10 == 3)) && x - u == 4)
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
                if(((x - 10 >= 0 && actual_matrix[x-10][y] / 100 != 1) || (x - 10 < 0) || (actual_matrix[x-9][y] % 10 == 3)) && x - u == 8)
                {
                  return destroy211;
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
        if(x - u == 2 && actual_matrix[u][v] == 0)
        {
          return normal;
        }
        if(x-2 >= 0 && actual_matrix[x-2][y] / 100 == 1 && x - u == 2)
        {
          return normal;
        }
        return impossible;
      }
    }
    else if(abs(y - v) == 1 && x - u == 1)
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
      if(abs(x - u) == 1 && abs(y - v) == 1 && actual_matrix[u][v] == 0)
      {
        return turn;
      }
      else if(y == v)
      {
        if(abs(u - x) == 2 && (actual_matrix[u][v] / 100 == 2 || actual_matrix[u][v] == 0))
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
        else if(u - x > 2 && actual_matrix[u][v] == 0)
        {
          for(int z = x+2; z <= u; z += 2)
          {
            if(actual_matrix[z-1][y] % 10 == 3 || actual_matrix[z][y] != 0)
            {
              return impossible;
            }
          }
          return destroy;
        }
        else if(x - u > 2 && actual_matrix[u][v] == 0)
        {
          for(int z = x-2; z >= u; z -= 2)
          {
            if(actual_matrix[z+1][y] % 10 == 3 || actual_matrix[z][y] != 0)
            {
              return impossible;
            }
          }
          return destroy;
        }
        return impossible;
      }
      else if(x == u && actual_matrix[u][v] == 0)
      {
        if(v > y)
        {
          for(int z = v+2; z < 14; z += 2)
          {
            if(actual_matrix[x][z-1] % 10 != 3 && actual_matrix[x][z] / 100 == 2 && actual_matrix[x][z+2] == 0  && actual_matrix[x][z-2] == 0)
            {
              return impossible;
            }
          }
          if(v - y == 2 && actual_matrix[x][y+1] % 10 != 3)
          {
            return normal;
          }
          else if(y+2 < 15 && actual_matrix[x][y+1] % 10 != 3 && actual_matrix[x][y+2] / 100 != 1)
          {
            if(y+4 < 15 && actual_matrix[x][y+3] % 10 != 3 && ((actual_matrix[x][y+2] == 0 && actual_matrix[x][y+4] / 100 == 2) || actual_matrix[x][y+4] == 0))
            {
              if(v - y == 4)
              {
                return destroy;
              }
              else if(y+6 < 15 && actual_matrix[x][y+5] % 10 != 3 && actual_matrix[x][y+6] / 100 != 1)
              {
                if(v - y == 6)
                {
                  return destroy;
                }
                else if(y+8 < 15 && actual_matrix[x][y+7] % 10 != 3 && ((actual_matrix[x][y+6] == 0 && actual_matrix[x][y+8] / 100 == 2) || ((actual_matrix[x][y+4] == 0 || actual_matrix[x][y+6] == 0) && actual_matrix[x][y+8] == 0)))
                {
                  if(v - y == 8)
                  {
                    return destroy;
                  }
                  else if(y+10 < 15 && actual_matrix[x][y+9] % 10 != 3 && actual_matrix[x][y+10] / 100 != 1)
                  {
                    if(v - y == 10)
                    {
                      return destroy;
                    }
                    else if(y+12 < 15 && actual_matrix[x][y+11] % 10 != 3 && ((actual_matrix[x][y+10] == 0 && actual_matrix[x][y+12] / 100 == 2) || ((actual_matrix[x][y+8] == 0 || actual_matrix[x][y+10] == 0) && actual_matrix[x][y+12] == 0)))
                    {
                      if(v - y == 12)
                      {
                        return destroy;
                      }
                      else if(v - y == 14 && actual_matrix[x][y+13] % 10 != 3)
                      {
                        return destroy;
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
            if(actual_matrix[x][z+1] % 10 != 3 && actual_matrix[x][z] / 100 == 2 && actual_matrix[x][z-2] == 0  && actual_matrix[x][z+2] == 0)
            {
              return impossible;
            }
          }
          if(y - v == 2 && actual_matrix[x][y-1] % 10 != 3)
          {
            return normal;
          }
          else if(y-2 >= 0 && actual_matrix[x][y-1] % 10 != 3 && actual_matrix[x][y-2] / 100 != 1)
          {
            if(y-4 >= 0 && actual_matrix[x][y-3] % 10 != 3 && ((actual_matrix[x][y-2] == 0 && actual_matrix[x][y-4] / 100 == 2) || actual_matrix[x][y-4] == 0))
            {
              if(y - v == 4)
              {
                return destroy;
              }
              else if(y-6 >= 0 && actual_matrix[x][y-5] % 10 != 3 && actual_matrix[x][y-6] / 100 != 1)
              {
                if(y - v == 6)
                {
                  return destroy;
                }
                else if(y-8 >= 0 && actual_matrix[x][y-7] % 10 != 3 && ((actual_matrix[x][y-6] == 0 && actual_matrix[x][y-8] / 100 == 2) || ((actual_matrix[x][y-4] == 0 || actual_matrix[x][y-6] == 0) && actual_matrix[x][y-8] == 0)))
                {
                  if(y - v == 8)
                  {
                    return destroy;
                  }
                  else if(y-10 >= 0 && actual_matrix[x][y-9] % 10 != 3 && actual_matrix[x][y-10] / 100 != 0)
                  {
                    if(y - v == 10)
                    {
                      return destroy;
                    }
                    else if(y-12 >= 0 && actual_matrix[x][y-11] % 10 != 3 && ((actual_matrix[x][y-10] == 0 && actual_matrix[x][y-12] / 100 == 2) || ((actual_matrix[x][y-8] == 0 || actual_matrix[x][y-10] == 0) && actual_matrix[x][y-12] == 0)))
                    {
                      if(y - v == 12)
                      {
                        return destroy;
                      }
                      else if(y - v == 14 && actual_matrix[x][y-13] % 10 != 3)
                      {
                        return destroy;
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
      if(abs(x - u) == 1 && abs(y - v) == 1 && actual_matrix[u][v] == 0)
      {
        return turn;
      }
      else if(x == u)
      {
        if(abs(y - v) == 2 && (actual_matrix[u][v] / 100 == 2 || actual_matrix[u][v] == 0))
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
        else if(v - y > 2 && actual_matrix[u][v] == 0)
        {
          for(int z = y+2; z <= v; z += 2)
          {
            if(actual_matrix[x][z-1] % 10 == 3 || actual_matrix[x][z] != 0)
            {
              return impossible;
            }
          }
          return destroy;
        }
        else if(y - v > 2 && actual_matrix[u][v] == 0)
        {
          for(int z = y-2; z >= v; z -= 2)
          {
            if(actual_matrix[x][z+1] % 10 == 3 || actual_matrix[x][z] != 0)
            {
              return impossible;
            }
          }
          return destroy;
        }
        return impossible;
      }
      else if(y == v && actual_matrix[u][v] == 0)
      {
        if(u > x)
        {
          for(int z = u+2; z < 14; z += 2)
          {
            if(actual_matrix[z-1][y] % 10 != 3 && actual_matrix[z][y] / 100 == 2 && actual_matrix[z+2][y] == 0 && actual_matrix[z-2][y] == 0)
            {
              return impossible;
            }
          }
          if(u - x == 2 && actual_matrix[x+1][y] % 10 != 3)
          {
            return normal;
          }
          else if(x+2 < 15 && actual_matrix[x+1][y] % 10 != 3 && actual_matrix[x+2][y] / 100 != 1)
          {
            if(x+4 < 15 && actual_matrix[x+3][y] % 10 != 3 && ((actual_matrix[x+2][y] == 0 && actual_matrix[x+4][y] / 100 == 2) || actual_matrix[x+4][y] == 0))
            {
              if(u - x == 4)
              {
                return destroy;
              }
              else if(x+6 < 15 && actual_matrix[x+5][y] % 10 != 3 && actual_matrix[x+6][y] / 100 != 1)
              {
                if(u - x == 6)
                {
                  return destroy;
                }
                else if(x+8 < 15 && actual_matrix[x+7][y] % 10 != 3 && ((actual_matrix[x+6][y] == 0 && actual_matrix[x+8][y] / 100 == 2) || ((actual_matrix[x+4][y] == 0 || actual_matrix[x+6][y] == 0) && actual_matrix[x+8][y] == 0)))
                {
                  if(u - x == 8)
                  {
                    return destroy;
                  }
                  else if(x+10 < 15 && actual_matrix[x+9][y] % 10 != 3 && actual_matrix[x+10][y] / 100 != 1)
                  {
                    if(u - x == 10)
                    {
                      return destroy;
                    }
                    else if(x+12 < 15 && actual_matrix[x+11][y] % 10 != 3 && ((actual_matrix[x+10][y] == 0 && actual_matrix[x+12][y] / 100 == 2) || ((actual_matrix[x+8][y] == 0 || actual_matrix[x+10][y] == 0) && actual_matrix[x+12][y] == 0)))
                    {
                      if(u - x == 12)
                      {
                        return destroy;
                      }
                      else if(u - x == 14 && actual_matrix[x+13][y] % 10 != 3)
                      {
                        return destroy;
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
            if(actual_matrix[z+1][y] % 10 != 3 && actual_matrix[z][y] / 100 == 2 && actual_matrix[z-2][y] == 0 && actual_matrix[z+2][y] == 0)
            {
              return impossible;
            }
          }
          if(x - u == 2 && actual_matrix[x-1][y] % 10 != 3)
          {
            return normal;
          }
          else if(x-2 >= 0 && actual_matrix[x-1][y] % 10 != 3 && actual_matrix[x-2][y] / 100 != 1)
          {
            if(x-4 >= 0 && actual_matrix[x-3][y] % 10 != 3 && ((actual_matrix[x-2][y] == 0 && actual_matrix[x-4][y] / 100 == 2) || actual_matrix[x-4][y] == 0))
            {
              if(x - u == 4)
              {
                return destroy;
              }
              else if(x-6 >= 0 && actual_matrix[x-5][y] % 10 != 3 && actual_matrix[x-6][y] / 100 != 1)
              {
                if(x - u == 6)
                {
                  return destroy;
                }
                else if(x-8 >= 0 && actual_matrix[x-7][y] % 10 != 3 && ((actual_matrix[x-6][y] == 0 && actual_matrix[x-8][y] / 100 == 2) || ((actual_matrix[x-4][y] == 0 || actual_matrix[x-6][y] == 0) && actual_matrix[x-8][y] == 0)))
                {
                  if(x - u == 8)
                  {
                    return destroy;
                  }
                  else if(x-10 >= 0 && actual_matrix[x-9][y] % 10 != 3 && actual_matrix[x-10][y] / 100 != 1)
                  {
                    if(x - u == 10)
                    {
                      return destroy;
                    }
                    else if(x-12 >= 0 && actual_matrix[x-11][y] % 10 != 3 && ((actual_matrix[x-10][y] == 0 && actual_matrix[x-12][y] / 100 == 2) || ((actual_matrix[x-8][y] == 0 || actual_matrix[x-10][y] == 0) && actual_matrix[x-12][y] == 0)))
                    {
                      if(x - u == 12)
                      {
                        return destroy;
                      }
                      else if(x - u == 14 && actual_matrix[x-13][y] % 10 != 3)
                      {
                        return destroy;
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
      if(abs(x - u) == 1 && abs(y - v) == 1 && actual_matrix[u][v] == 0)
      {
        return turn;
      }
      else if(y == v)
      {
        if(abs(u - x) == 2 && (actual_matrix[u][v] / 100 == 1 || actual_matrix[u][v] == 0))
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
        else if(u - x > 2 && actual_matrix[u][v] == 0)
        {
          for(int z = x+2; z <= u; z += 2)
          {
            if(actual_matrix[z-1][y] % 10 == 3 || actual_matrix[z][y] != 0)
            {
              return impossible;
            }
          }
          return destroy;
        }
        else if(x - u > 2 && actual_matrix[u][v] == 0)
        {
          for(int z = x-2; z >= u; z -= 2)
          {
            if(actual_matrix[z+1][y] % 10 == 3 || actual_matrix[z][y] != 0)
            {
              return impossible;
            }
          }
          return destroy;
        }
        return impossible;
      }
      else if(x == u && actual_matrix[u][v] == 0)
      {
        if(v > y)
        {
          for(int z = v+2; z < 14; z += 2)
          {
            if(actual_matrix[x][z-1] % 10 != 3 && actual_matrix[x][z] / 100 == 1 && actual_matrix[x][z+2] == 0 && actual_matrix[x][z-2] == 0)
            {
              return impossible;
            }
          }
          if(v - y == 2 && actual_matrix[x][y+1] % 10 != 3)
          {
            return normal;
          }
          else if(y+2 < 15 && actual_matrix[x][y+1] % 10 != 3 && actual_matrix[x][y+2] / 100 != 2)
          {
            if(y+4 < 15 && actual_matrix[x][y+3] % 10 != 3 && ((actual_matrix[x][y+2] == 0 && actual_matrix[x][y+4] / 100 == 1) || actual_matrix[x][y+4] == 0))
            {
              if(v - y == 4)
              {
                return destroy;
              }
              else if(y+6 < 15 && actual_matrix[x][y+5] % 10 != 3 && actual_matrix[x][y+6] / 100 != 2)
              {
                if(v - y == 6)
                {
                  return destroy;
                }
                else if(y+8 < 15 && actual_matrix[x][y+7] % 10 != 3 && ((actual_matrix[x][y+6] == 0 && actual_matrix[x][y+8] / 100 == 1) || ((actual_matrix[x][y+4] == 0 || actual_matrix[x][y+6] == 0) && actual_matrix[x][y+8] == 0)))
                {
                  if(v - y == 8)
                  {
                    return destroy;
                  }
                  else if(y+10 < 15 && actual_matrix[x][y+9] % 10 != 3 && actual_matrix[x][y+10] / 100 != 2)
                  {
                    if(v - y == 10)
                    {
                      return destroy;
                    }
                    else if(y+12 < 15 && actual_matrix[x][y+11] % 10 != 3 && ((actual_matrix[x][y+10] == 0 && actual_matrix[x][y+12] / 100 == 1) || ((actual_matrix[x][y+8] == 0 || actual_matrix[x][y+10] == 0) && actual_matrix[x][y+12] == 0)))
                    {
                      if(v - y == 12)
                      {
                        return destroy;
                      }
                      else if(v - y == 14 && actual_matrix[x][y+13] % 10 != 3)
                      {
                        return destroy;
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
            if(actual_matrix[x][z+1] % 10 != 3 && actual_matrix[x][z] / 100 == 1 && actual_matrix[x][z-2] == 0  && actual_matrix[x][z+2] == 0)
            {
              return impossible;
            }
          }
          if(y - v == 2 && actual_matrix[x][y-1] % 10 != 3)
          {
            return normal;
          }
          else if(y-2 >= 0 && actual_matrix[x][y-1] % 10 != 3 && actual_matrix[x][y-2] / 100 != 2)
          {
            if(y-4 >= 0 && actual_matrix[x][y-3] % 10 != 3 && ((actual_matrix[x][y-2] == 0 && actual_matrix[x][y-4] / 100 == 1) || actual_matrix[x][y-4] == 0))
            {
              if(y - v == 4)
              {
                return destroy;
              }
              else if(y-6 >= 0 && actual_matrix[x][y-5] % 10 != 3 && actual_matrix[x][y-6] / 100 != 2)
              {
                if(y - v == 6)
                {
                  return destroy;
                }
                else if(y-8 >= 0 && actual_matrix[x][y-7] % 10 != 3 && ((actual_matrix[x][y-6] == 0 && actual_matrix[x][y-8] / 100 == 1) || ((actual_matrix[x][y-4] == 0 || actual_matrix[x][y-6] == 0) && actual_matrix[x][y-8] == 0)))
                {
                  if(y - v == 8)
                  {
                    return destroy;
                  }
                  else if(y-10 >= 0 && actual_matrix[x][y-9] % 10 != 3 && actual_matrix[x][y-10] / 100 != 2)
                  {
                    if(y - v == 10)
                    {
                      return destroy;
                    }
                    else if(y-12 >= 0 && actual_matrix[x][y-11] % 10 != 3 && ((actual_matrix[x][y-10] == 0 && actual_matrix[x][y-12] / 100 == 1) || ((actual_matrix[x][y-8] == 0 || actual_matrix[x][y-6] == 0) && actual_matrix[x][y-12] == 0)))
                    {
                      if(y - v == 12)
                      {
                        return destroy;
                      }
                      else if(y - v == 14 && actual_matrix[x][y-13] % 10 != 3)
                      {
                        return destroy;
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
      if(abs(x - u) == 1 && abs(y - v) == 1 && actual_matrix[u][v] == 0)
      {
        return turn;
      }
      else if(x == u)
      {
        if(abs(y - v) == 2 && (actual_matrix[u][v] / 100 == 1 || actual_matrix[u][v] == 0))
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
        else if(v - y > 2 && actual_matrix[u][v] == 0)
        {
          for(int z = y+2; z <= v; z += 2)
          {
            if(actual_matrix[x][z-1] % 10 == 3 || actual_matrix[x][z] != 0)
            {
              return impossible;
            }
          }
          return destroy;
        }
        else if(y - v > 2 && actual_matrix[u][v] == 0)
        {
          for(int z = y-2; z >= v; z -= 2)
          {
            if(actual_matrix[x][z+1] % 10 == 3 || actual_matrix[x][z] != 0)
            {
              return impossible;
            }
          }
          return destroy;
        }
        return impossible;
      }
      else if(y == v && actual_matrix[u][v] == 0)
      {
        if(u > x)
        {
          for(int z = u+2; z < 14; z += 2)
          {
            if(actual_matrix[z-1][y] % 10 != 3 && actual_matrix[z][y] / 100 == 1 && actual_matrix[z+2][y] == 0 && actual_matrix[z-2][y] == 0)
            {
              return impossible;
            }
          }
          if(u - x == 2 && actual_matrix[x+1][y] % 10 != 3)
          {
            return normal;
          }
          else if(x+2 < 15 && actual_matrix[x+1][y] % 10 != 3 && actual_matrix[x+2][y] / 100 != 2)
          {
            if(x+4 < 15 && actual_matrix[x+3][y] % 10 != 3 && ((actual_matrix[x+2][y] == 0 && actual_matrix[x+4][y] / 100 == 1) || actual_matrix[x+4][y] == 0))
            {
              if(u - x == 4)
              {
                return destroy;
              }
              else if(x+6 < 15 && actual_matrix[x+5][y] % 10 != 3 && actual_matrix[x+6][y] / 100 != 2)
              {
                if(u - x == 6)
                {
                  return destroy;
                }
                else if(x+8 < 15 && actual_matrix[x+7][y] % 10 != 3 && ((actual_matrix[x+6][y] == 0 && actual_matrix[x+8][y] / 100 == 1) || ((actual_matrix[x+4][y] == 0 || actual_matrix[x+6][y] == 0) && actual_matrix[x+8][y] == 0)))
                {
                  if(u - x == 8)
                  {
                    return destroy;
                  }
                  else if(x+10 < 15 && actual_matrix[x+9][y] % 10 != 3 && actual_matrix[x+10][y] / 100 != 2)
                  {
                    if(u - x == 10)
                    {
                      return destroy;
                    }
                    else if(x+12 < 15 && actual_matrix[x+11][y] % 10 != 3 && ((actual_matrix[x+10][y] == 0 && actual_matrix[x+12][y] / 100 == 1) || ((actual_matrix[x+8][y] == 0 || actual_matrix[x+10][y] == 0) && actual_matrix[x+12][y] == 0)))
                    {
                      if(u - x == 12)
                      {
                        return destroy;
                      }
                      else if(u - x == 14 && actual_matrix[x+13][y] % 10 != 3)
                      {
                        return destroy;
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
            if(actual_matrix[z+1][y] % 10 != 3 && actual_matrix[z][y] / 100 == 1 && actual_matrix[z-2][y] == 0  && actual_matrix[z+2][y] == 0)
            {
              return impossible;
            }
          }
          if(x - u == 2 && actual_matrix[x-1][y] % 10 != 3)
          {
            return normal;
          }
          else if(x-2 >= 0 && actual_matrix[x-1][y] % 10 != 3 && actual_matrix[x-2][y] / 100 != 2)
          {
            if(x-4 >= 0 && actual_matrix[x-3][y] % 10 != 3 && ((actual_matrix[x-2][y] == 0 && actual_matrix[x-4][y] / 100 == 1) || actual_matrix[x-4][y] == 0))
            {
              if(x - u == 4)
              {
                return destroy;
              }
              else if(x-6 >= 0 && actual_matrix[x-5][y] % 10 != 3 && actual_matrix[x-6][y] / 100 != 2)
              {
                if(x - u == 6)
                {
                  return destroy;
                }
                else if(x-8 >= 0 && actual_matrix[x-7][y] % 10 != 3 && ((actual_matrix[x-6][y] == 0 && actual_matrix[x-8][y] / 100 == 1) || ((actual_matrix[x-4][y] == 0 || actual_matrix[x-6][y] == 0) && actual_matrix[x-8][y] == 0)))
                {
                  if(x - u == 8)
                  {
                    return destroy;
                  }
                  else if(x-10 >= 0 && actual_matrix[x-9][y] % 10 != 3 && actual_matrix[x-10][y] / 100 != 2)
                  {
                    if(x - u == 10)
                    {
                      return destroy;
                    }
                    else if(x-12 >= 0 && actual_matrix[x-11][y] % 10 != 3 && ((actual_matrix[x-10][y] == 0 && actual_matrix[x-12][y] / 100 == 1) || ((actual_matrix[x-8][y] == 0 || actual_matrix[x-10][y] == 0) && actual_matrix[x-12][y] == 0)))
                    {
                      if(x - u == 12)
                      {
                        return destroy;
                      }
                      else if(x - u == 14 && actual_matrix[x-13][y] % 10 != 3)
                      {
                        return destroy;
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
  else if(apply == destroy110 || apply == destroy111)
  {
    (*actual_matrix)[x+2][y] = 0;
    (*actual_matrix)[x+4][y] = (*actual_matrix)[x][y];
    (*actual_matrix)[x][y] = 0;
    if(apply == destroy111)
    {
      draw(*actual_matrix);
      SDL_Delay(200);
      (*actual_matrix)[x+4][y] = 0;
      SDL_Delay(200);
      (*actual_matrix)[x+6][y] = 0;
      (*actual_matrix)[x+8][y] = temp;
    }
  }
  else if(apply == destroy210 || apply == destroy211)
  {
    (*actual_matrix)[x-2][y] = 0;
    (*actual_matrix)[x-4][y] = (*actual_matrix)[x][y];
    (*actual_matrix)[x][y] = 0;
    if(apply == destroy211)
    {
      draw(*actual_matrix);
      SDL_Delay(200);
      (*actual_matrix)[x-4][y] = 0;
      SDL_Delay(200);
      (*actual_matrix)[x-6][y] = 0;
      (*actual_matrix)[x-8][y] = temp;
    }
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
            (*actual_matrix)[x][z-2] = 0;
            (*actual_matrix)[x][z] = 0;
            (*actual_matrix)[x][z+2] = temp;
            draw(*actual_matrix);
            SDL_Delay(400);
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
            (*actual_matrix)[x][z+2] = 0;
            (*actual_matrix)[x][z] = 0;
            (*actual_matrix)[x][z-2] = temp;
            draw(*actual_matrix);
            SDL_Delay(400);
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
            (*actual_matrix)[z-2][y] = 0;
            (*actual_matrix)[z][y] = 0;
            (*actual_matrix)[z+2][y] = temp;
            draw(*actual_matrix);
            SDL_Delay(400);
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
            (*actual_matrix)[z+2][y] = 0;
            (*actual_matrix)[z][y] = 0;
            (*actual_matrix)[z-2][y] = temp;
            draw(*actual_matrix);
            SDL_Delay(400);
          }
          else if((*actual_matrix)[z][y] == temp)
          {
            (*actual_matrix)[z][y] = 0;
          }
        }
      }
    }
    (*actual_matrix)[u][v] = temp;
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
    actual_player++;
  }
}
