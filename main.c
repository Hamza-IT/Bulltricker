#include <stdio.h>
#include <stdlib.h>
#include "init_mat.h"
#include "gui.h"
#include "move.h"

int main(int argc, char **argv)
{
  int **actual_matrix = initMat();
  createBoard(actual_matrix);
  return 0;
}
