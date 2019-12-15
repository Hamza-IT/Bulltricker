#include <stdio.h>
#include <stdlib.h>
#include "move.h"
#include "init_mat.h"

int actual_player = 0;

typedef enum _bool
{
  false,
  true
} bool;

// Still need to do queen and pawn movement + rotation...

bool check_move(int x, int y, int u, int v, int **actual_matrix)
{
  if(actual_matrix[x][y] == 0 || actual_matrix[x][y] == -1 || actual_matrix[x][y] == -2)
  {
    printf("Source is not a playable piece...");
    return false;
  }
  else if((actual_matrix[x][y] / 100 == 1 || actual_matrix[x][y] / 10 == 1) && actual_player % 2 == 0)
  {
    printf("Not blacks turn...");
    return false;
  }
  else if((actual_matrix[x][y] / 100 == 2 || actual_matrix[x][y] / 10 == 2) && actual_player % 2 == 1)
  {
    printf("Not whites turn...");
    return false;
  }
  else if(actual_matrix[u][v] == -1 || (x == u && y == v) || (actual_matrix[x][y] / 100 == actual_matrix[u][v] / 100 && actual_matrix[x][y] % 10 != 3))
  {
    printf("Destination is not a playable tile...");
    return false;
  }
  else if(actual_matrix[x][y] % 100 != 13 && actual_matrix[x][y] != 23 && actual_matrix[u][v] == -2)
  {
    printf("Tile is for king only...");
    return false;
  }
  else if(actual_player == 0)
  {
    if(actual_matrix[x][y] == 220)
    {
      if(u - x == 1 && abs(y - v) == 1)
      {
        actual_player++;
        printf("Possible move!");
        return true;
      }
      else
      {
        printf("Impossible move!");
        return false;
      }
    }
    else if(actual_matrix[x][y] == 210)
    {
      if(x - u == 1 && abs(y - v) == 1)
      {
        actual_player++;
        printf("Possible move!");
        return true;
      }
      else if(x - u == 2 && y == v)
      {
        actual_player++;
        printf("Possible move!");
        return true;
      }
      else
      {
        printf("Impossible move!");
        return false;
      }
    }
    else if(actual_matrix[x][y] == 211)
    {
      if((x - u == 2 || x - u == 4) && y == v)
      {
        actual_player++;
        printf("Possible move!");
        return true;
      }
      else
      {
        printf("Impossible move!");
        return false;
      }
    }
    else
    {
      printf("Impossible move!");
      return false;
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
        return true;
      }
      else
      {
        printf("Impossible move!");
        return false;
      }
    }
    else if(actual_matrix[x][y] == 110)
    {
      if(u - x == 1 && abs(y - v) == 1)
      {
        actual_player++;
        printf("Possible move!");
        return true;
      }
      else if(u - x == 2 && y == v)
      {
        actual_player++;
        printf("Possible move!");
        return true;
      }
      else
      {
        printf("Impossible move!");
        return false;
      }
    }
    else if(actual_matrix[x][y] == 111)
    {
      if((u - x == 2 || u - x == 4) && y == v)
      {
        actual_player++;
        printf("Possible move!");
        return true;
      }
      else
      {
        printf("Impossible move!");
        return false;
      }
    }
    else
    {
      printf("Impossible move!");
      return false;
    }
  }
  else if(actual_matrix[x][y] == 23)
  {
    if(actual_matrix[u][v] != -2)
    {
      printf("Impossible move!");
      return false;
    }
    else if(x == u)
    {
      if((v - y == 2 && actual_matrix[x][y + 1] == 0) || (y - v == 2 && actual_matrix[x][y - 1] == 0))
      {
        actual_player++;
        printf("Possible move!");
        return true;
      }
      else
      {
        printf("Impossible move!");
        return false;
      }
    }
    else if(y == v)
    {
      if((x - u == 2 && actual_matrix[x - 1][y] == 0) || (u - x == 2 && actual_matrix[x + 1][y] == 0))
      {
        actual_player++;
        printf("Possible move!");
        return true;
      }
      else
      {
        printf("Impossible move!");
        return false;
      }
    }
    else
    {
      printf("Impossible move!");
      return false;
    }
  }
  else if(actual_matrix[x][y] == 13)
  {
    if(actual_matrix[u][v] != -2)
    {
      printf("Impossible move!");
      return false;
    }
    else if(x == u)
    {
      if((v - y == 2 && actual_matrix[x][y + 1] == 0) || (y - v == 2 && actual_matrix[x][y - 1] == 0))
      {
        actual_player++;
        printf("Possible move!");
        return true;
      }
      else
      {
        printf("Impossible move!");
        return false;
      }
    }
    else if(y == v)
    {
      if((x - u == 2 && actual_matrix[x - 1][y] == 0) || (u - x == 2 && actual_matrix[x + 1][y] == 0))
      {
        actual_player++;
        printf("Possible move!");
        return true;
      }
      else
      {
        printf("Impossible move!");
        return false;
      }
    }
    else
    {
      printf("Impossible move!");
      return false;
    }
  }
  actual_player++;
  return true;
}

void apply_move(int x, int y, int u, int v, int ***actual_matrix)
{
  if(check_move(x, y, u, v, *actual_matrix))
  {
    (*actual_matrix)[u][v] = (*actual_matrix)[x][y];
    (*actual_matrix)[x][y] = 0;
  }
}
