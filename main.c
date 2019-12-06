#include <stdio.h>
#include <stdlib.h>
#include "game_objects.h"
#include "initMat.h"

int row_count;
int column_count;
int king_count;
int queen_count;
int pawn_count;
int **initial_matrix;

int main()
{
  initial_matrix = initMat(&row_count, &column_count, &king_count, &queen_count, &pawn_count);
  return 0;
}
