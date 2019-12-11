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

bool check_move(int x, int y, int u, int v, int **actual_matrix)
{
  if(x < 0 || x >= 15 || y >= 15 || y < 0 || u < 0 || u >= 15 || v >= 15 || v < 0)
  {
    printf("Out of bounds...");
    return false;
  }
  if(actual_matrix[u][v] == -1)
  {
    printf("Destination is a not playable tile...");
    return false;
  }
  if(actual_matrix == initial_matrix && actual_player == 0)
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
    if(actual_matrix[x][y] == 210)
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
    if(actual_matrix[x][y] == 211)
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
    printf("Impossible move!");
    return false;
  }
  else if(actual_matrix[x][y] == 23 && actual_player % 2 == 0)
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
    }
    else if(y == v)
    {
      if((x - u == 2 && actual_matrix[x - 1][y] == 0) || (u - x == 2 && actual_matrix[x + 1][y] == 0))
      {
        actual_player++;
        printf("Possible move!");
        return true;
      }
    }
    else
    {
      printf("Impossible move!");
      return false;
    }
  }
}

void apply_move(int x, int y, int u, int v, int ***actual_matrix)
{
  if(x != u || y != v)
  {
    (*actual_matrix)[u][v] = (*actual_matrix)[x][y];
    (*actual_matrix)[x][y] = 0;
  }
}
