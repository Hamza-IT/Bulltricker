#include <stdio.h>
#include <stdlib.h>
#include "init_mat.h"

int **initMat()
{
  FILE *fptr = fopen("Bulltricker_Data/general.txt", "r");
  fscanf(fptr, "%d %d %d %d %d", &row_count, &column_count, &king_count, &queen_count, &pawn_count);
  initial_matrix = malloc(row_count * sizeof(int*));
  for(int i = 0; i < row_count; i++)
  {
    initial_matrix[i] = malloc(column_count * sizeof(int));
  }
  copy_matrix = malloc(row_count * sizeof(int*));
  for(int i = 0; i < row_count; i++)
  {
    copy_matrix[i] = malloc(column_count * sizeof(int));
  }
  for(int i = 0; i < row_count; i++)
  {
    for(int j = 0; j < column_count; j++)
    {
      int x;
      fscanf(fptr, "%d", &x);
      initial_matrix[i][j] = x;
      copy_matrix[i][j] = x;
    }
  }
  fclose(fptr);
  return initial_matrix;
}
