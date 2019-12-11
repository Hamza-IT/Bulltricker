#include <stdio.h>
#include <stdlib.h>
#include "init_mat.h"

int row_count;
int column_count;
int king_count;
int queen_count;
int pawn_count;
int **initial_matrix;

int **initMat()
{
  FILE *fptr = fopen("general.txt", "r");
  fscanf(fptr, "%d %d %d %d %d", &row_count, &column_count, &king_count, &queen_count, &pawn_count);
  initial_matrix = malloc(row_count * sizeof(int*));
  for(int i = 0; i < row_count; i++)
  {
    initial_matrix[i] = malloc(column_count * sizeof(int));
  }
  for(int i = 0; i < row_count; i++)
  {
    for(int j = 0; j < column_count; j++)
    {
      fscanf(fptr, "%d", *(initial_matrix + i) + j);
    }
  }
  fclose(fptr);
  return initial_matrix;
}
